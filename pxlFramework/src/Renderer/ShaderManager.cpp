#include "ShaderManager.h"

#include "Utils/FileSystem.h"

namespace pxl
{
    ShaderManager::ShaderManager(const ShaderManagerConfig& config)
        : m_Config(config)
    {
        // Create the cache directory (even if we don't use it)
        std::filesystem::create_directory(m_Config.CacheDirectory);
    }

    void ShaderManager::Add(const ShaderFile& shader)
    {
        // Validate file extension
        auto extString = shader.FileName.extension().string();
        if (!ValidateFileExtension(extString))
        {
            PXL_LOG_ERROR("Failed to load shader, file extension '{}' isn't supported", extString);
            return;
        }

        // Check for the shader from all shader directories
        auto sourcePath = FindShaderFile(shader.FileName);

        if (shader.FileName.extension() == ".spv")
        {
            // Shader is already compiled, Load and queue for creation
            auto spirv = FileSystem::LoadSPIRV(sourcePath);
            m_CreationQueue.push_back({ shader.Stage, spirv, sourcePath });
        }

        // Check the shader cache
        auto cacheFilePath = GenerateShaderCachePath(shader.FileName);
        bool cacheLoaded = false;
        if (std::filesystem::exists(cacheFilePath) && m_Config.UseCache)
        {
            // Load cache spirv
            auto spirv = FileSystem::LoadSPIRV(cacheFilePath);

            if (!spirv.empty())
            {
                cacheLoaded = true;
                PXL_LOG_INFO("Loaded cached shader '{}'", cacheFilePath.filename().string());

                // Queue shader for creation
                m_CreationQueue.push_back({ shader.Stage, spirv, sourcePath });
            }
        }

        if (cacheLoaded)
            return;

        // If cache isn't available, queue the shader for compilation
        m_CompilationQueue.push_back({ sourcePath, shader });
    }

    void ShaderManager::Add(std::vector<ShaderFile>& shaders)
    {
        for (auto& shader : shaders)
        {
            Add(shader);
        }
    }

    std::shared_ptr<Shader> ShaderManager::Get(const std::filesystem::path& fileName)
    {
        if (!m_Shaders.contains(fileName))
        {
            PXL_LOG_ERROR("Shader manager does not have shader '{}'", fileName.string());
            return nullptr;
        }

        return m_Shaders.at(fileName);
    }

    void ShaderManager::ReloadAll()
    {
        InitCompiler();

        for (auto& [id, shader] : m_Shaders)
            Reload(shader);

        ShutdownCompiler();
    }

    void ShaderManager::CompileAll(GraphicsDevice& graphicsDevice)
    {
        InitCompiler();

        for (const auto& [sourcePath, shaderFile] : m_CompilationQueue)
        {
            // Load glsl file
            auto glsl = FileSystem::LoadGLSL(sourcePath);

            // Compile glsl to spirv using ShaderCompiler
            auto spirv = m_Compiler->CompileGLSLToSPIRV(shaderFile.Stage, glsl, shaderFile.FileName.string());

            // Cache the shader to disk if compilation succeeded
            if (m_Config.UseCache && !spirv.empty())
                CacheShaderToDisk(spirv, shaderFile.FileName);

            // Queue shader creation
            m_CreationQueue.push_back({ shaderFile.Stage, spirv, sourcePath });
        }

        m_CompilationQueue.clear();

        for (const auto& specs : m_CreationQueue)
        {
            m_Shaders[specs.SourcePath.filename()] = graphicsDevice.CreateShader(specs);
            PXL_LOG_INFO("Finished loading shader '{}'", specs.SourcePath.filename().string());
        }

        m_CreationQueue.clear();

        ShutdownCompiler();
    }

    std::filesystem::path ShaderManager::GenerateShaderCachePath(const std::filesystem::path& fileName)
    {
        auto fileNameString = fileName.string();
        std::replace(fileNameString.begin(), fileNameString.end(), '.', '_');
        std::filesystem::path cachePath = m_Config.CacheDirectory / fileNameString;
        cachePath.replace_filename(fileNameString + ".cache");

        return cachePath;
    }

    void ShaderManager::ClearCache()
    {
        for (const auto& entry : std::filesystem::directory_iterator(m_Config.CacheDirectory))
        {
            std::filesystem::remove_all(entry.path());
            PXL_LOG_INFO("Deleted '{}'", entry.path().string());
        }

        PXL_LOG_INFO("Shader cache deleted");
    }

    bool ShaderManager::ValidateFileExtension(const std::string& shaderExt)
    {
        // Load GLSL from disk and compile to SPIRV
        std::vector<std::filesystem::path> validExtensions = {
            ".spv",
            ".vert",
            ".frag",
            ".geom",
            ".tesc",
            ".tese",
            ".comp",
        };

        bool extensionIsValid = false;
        for (const auto& ext : validExtensions)
        {
            if (shaderExt == ext)
            {
                extensionIsValid = true;
                break;
            }
        }

        return extensionIsValid;
    }

    bool ShaderManager::Reload(const std::shared_ptr<Shader>& shader)
    {
        // Get the shader from storage
        auto shaderSpecs = shader->GetSpecs();
        auto shaderFilePath = shaderSpecs.SourcePath;

        // Check if the shader file still exists from it's source path
        if (!std::filesystem::exists(shaderFilePath))
        {
            PXL_LOG_ERROR("Failed to reload shader '{}', shader file does not exist.", shaderFilePath.string());
            return false;
        }

        // Load the shader file from disk and recompile if necessary
        std::vector<uint32_t> spirv;
        if (shaderFilePath.extension() == ".spv")
        {
            spirv = FileSystem::LoadSPIRV(shaderFilePath);
        }
        else
        {
            InitCompiler();
            auto glsl = FileSystem::LoadGLSL(shaderFilePath);
            spirv = m_Compiler->CompileGLSLToSPIRV(shaderSpecs.Stage, glsl, shaderFilePath.filename().string());
        }

        // Cache the shader
        if (m_Config.UseCache)
            CacheShaderToDisk(spirv, shaderFilePath.filename());

        // Reload the shader using the newly loaded SPIRV
        shader->Reload(spirv);
        PXL_LOG_INFO("Reloaded shader '{}'", shaderFilePath.filename().string());

        return true;
    }

    void ShaderManager::CacheShaderToDisk(const std::vector<uint32_t> code, const std::filesystem::path& fileName)
    {
        auto cacheFilePath = GenerateShaderCachePath(fileName);
        std::ofstream outStream(cacheFilePath, std::ios::out | std::ios::binary);
        for (uint32_t word : code)
        {
            outStream.write(reinterpret_cast<const char*>(&word), sizeof(uint32_t));
        }

        PXL_LOG_INFO("Cached shader to '{}'", cacheFilePath.string());
    }

    std::filesystem::path ShaderManager::FindShaderFile(const std::filesystem::path& fileName)
    {
        for (auto& directory : m_Config.ShaderDirectories)
        {
            auto path = directory / fileName;
            if (std::filesystem::exists(path))
            {
                return path;
            }
        }

        return std::filesystem::path();
    }

    void ShaderManager::InitCompiler()
    {
        if (!m_Compiler)
            m_Compiler = std::make_unique<ShaderCompiler>();
    }
}