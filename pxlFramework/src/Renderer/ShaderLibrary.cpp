#include "ShaderLibrary.h"

namespace pxl
{
    void ShaderLibrary::Add(const std::string& filename, const std::shared_ptr<Shader> shader) // could this be a reference
    {
        s_Shaders[filename] = shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& filename)
    {
        if (s_Shaders.find(filename) != s_Shaders.end())
            return s_Shaders[filename];
        else
            PXL_LOG_ERROR(LogArea::Renderer, "Failed to find shader '{}' in Shader Library", filename);

        return nullptr;
    }
}