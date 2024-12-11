#pragma once

#include "Shader.h"

namespace pxl
{
    class ShaderManager
    {
    public:
        // Load a shader and caches it with its filename
        static std::shared_ptr<Shader> LoadFromGLSL(const std::filesystem::path& path, ShaderStage stage);
        static std::shared_ptr<Shader> LoadFromSPIRV(const std::filesystem::path& path, ShaderStage stage);

        // Get a cached shader
        static std::shared_ptr<Shader> Get(const std::string& filename);

        // Retrieve a read-only reference of the shader cache
        static const std::unordered_map<std::string, std::shared_ptr<Shader>>& GetCache() { return s_ShaderCache; }

    private:
        static inline std::unordered_map<std::string, std::shared_ptr<Shader>> s_ShaderCache;
    };
}