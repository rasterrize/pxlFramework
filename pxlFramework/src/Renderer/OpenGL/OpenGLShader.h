#pragma once

#include "../Shader.h"

namespace pxl
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(ShaderStage stage, const std::string& glslSrc);
        ~OpenGLShader();

        virtual void Reload() override;
        
        virtual ShaderStage GetShaderStage() const override { return m_ShaderStage; }

        uint32_t GetID() const { return m_RendererID; }
    private:
        void Compile(const std::string& glslSrc);

        static uint32_t ShaderStageToOGLShaderStage(ShaderStage stage);
    private:
        uint32_t m_RendererID = 0;
        ShaderStage m_ShaderStage;
    };
}