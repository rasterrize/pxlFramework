#pragma once

namespace pxl
{
    enum class ShaderStage
    {
        None,
        Vertex,
        Fragment,
        Geometry,
        Tessellation,
    };
    
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Reload() = 0;

        virtual ShaderStage GetShaderStage() const = 0; 

        static std::shared_ptr<Shader> Create(ShaderStage stage, const std::string& glslSrc);
        static std::shared_ptr<Shader> Create(ShaderStage stage, const std::vector<char>& sprvBin);
    };
}