#pragma once

#include "../Shader.h"

#include <glm/mat4x4.hpp>

namespace pxl
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertSrc, const std::string& fragSrc);
        ~OpenGLShader();

        virtual void Bind() override;
        virtual void Unbind() override;


        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
    private:
        int GetUniformLocation(const std::string& name) const;
    private:
        unsigned int m_RendererID;
        mutable std::unordered_map<std::string, int> m_UniformCache;
    };
}