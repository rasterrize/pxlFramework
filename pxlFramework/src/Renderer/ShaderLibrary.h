#pragma once

#include "Shader.h"

namespace pxl
{
    class ShaderLibrary
    {
    public:
        static void Add(const std::string& filename, const std::shared_ptr<Shader> shader);
        static std::shared_ptr<Shader> Get(const std::string& filename);
    private:
        static std::unordered_map<std::string, std::shared_ptr<Shader>> s_Shaders;
    };
}