#pragma once

#include "GraphicsDevice.h"
#include "Shader.h"
#include "ShaderCompiler.h"

namespace pxl
{
    struct ShaderFile
    {
        std::filesystem::path FileName;
        ShaderStage Stage = {};
    };

    struct ShaderManagerConfig
    {
        /// @brief A list of search directories for shader files
        std::vector<std::filesystem::path> ShaderDirectories;

        /// @brief The directory to store cached shader files
        std::filesystem::path CacheDirectory;

        /// @brief Whether or not to cache compiled shaders to disk
        bool UseCache = false;
    };

    /// @brief A class used for handling various management processes for shaders,
    /// including shader storage, compilation, creation, and caching.
    class ShaderManager
    {
    public:
        ShaderManager(const ShaderManagerConfig& config);

        /// @brief Queue a shader for compilation/creation
        void Add(const ShaderFile& shader);

        /// @brief Queues a list of shaders for compilation/creation
        void Add(std::vector<ShaderFile>& shaderList);

        /// @brief Retrieve a shader from the shader manager
        std::shared_ptr<Shader> Get(const std::filesystem::path& fileName);

        /// @brief Reload a specific shader. The shader should already been added to the shader manager.
        bool Reload(const std::shared_ptr<Shader>& shader);

        /// @brief Reload all the shaders stored in this shader manager.
        void ReloadAll();

        void CompileAll(GraphicsDevice& graphicsDevice);

        // TODO
        void CompileAllAsync() {}

        void EnableCache() { m_Config.UseCache = true; }
        void DisableCache() { m_Config.UseCache = false; }

        /// @brief Add a shader directory used for finding shader files
        void AddShaderDirectory(const std::filesystem::path& path) { m_Config.ShaderDirectories.push_back(path); }

        /// @brief Set the cache directory for compiled shaders
        void SetCacheDirectory(const std::filesystem::path& path) { m_Config.CacheDirectory = path; }

        /// @brief Completely wipes the shader cache directory
        void ClearCache();

    private:
        std::filesystem::path GenerateShaderCachePath(const std::filesystem::path& fileName);
        void CacheShaderToDisk(const std::vector<uint32_t> code, const std::filesystem::path& fileName);
        bool ValidateFileExtension(const std::string& shaderExt);
        std::filesystem::path FindShaderFile(const std::filesystem::path& fileName);

    private:
        ShaderManagerConfig m_Config = {};

        std::vector<std::pair<std::filesystem::path, ShaderFile>> m_CompilationQueue;
        std::vector<ShaderSpecs> m_CreationQueue;

        // Stores shaders using a unique string identifier (usually the shader's file name)
        std::unordered_map<std::filesystem::path, std::shared_ptr<Shader>> m_Shaders;

        ShaderCompiler m_Compiler = {};
    };
}