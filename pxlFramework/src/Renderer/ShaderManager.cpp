#include "ShaderManager.h"

#include "Utils/FileSystem.h"

namespace pxl
{

    std::shared_ptr<Shader> ShaderManager::LoadFromGLSL(const std::filesystem::path& path, ShaderStage stage)
    {
        auto shader = Shader::Create(stage, FileSystem::LoadGLSL(path));

        PXL_ASSERT(shader);

        s_ShaderCache[path.filename().string()] = shader;

        return shader;
    }

    std::shared_ptr<Shader> ShaderManager::LoadFromSPIRV(const std::filesystem::path& path, ShaderStage stage)
    {
        auto shader = Shader::Create(stage, FileSystem::LoadSPIRV(path));

        PXL_ASSERT(shader);

        s_ShaderCache[path.filename().string()] = shader;

        return shader;
    }

    std::shared_ptr<Shader> ShaderManager::Get(const std::string& filename)
    {
        PXL_ASSERT_MSG(s_ShaderCache.contains(filename), "Shader cache doesn't contain shader");

        return s_ShaderCache[filename];
    }
}