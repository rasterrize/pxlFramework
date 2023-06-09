#include "ShaderLibrary.h"

namespace pxl
{
    std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_Shaders;

    void ShaderLibrary::Add(const std::string& filename, const std::shared_ptr<Shader> shader) // could this be a reference
    {
        s_Shaders[filename] = shader;
    }

    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& filename)
    {
        if (s_Shaders.find(filename) != s_Shaders.end())
            return s_Shaders[filename];
        else
            Logger::LogError("Failed to find shader '" + filename + "' in Shader Library");
            return nullptr; 
    }
}