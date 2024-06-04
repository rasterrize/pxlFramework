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

        virtual void Reload() override;

        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformInt1(const std::string& name, int value) override;
        virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) override;
    private:
        void Compile(const std::string& vertSrc, const std::string& fragSrc);
        int GetUniformLocation(const std::string& name) const;
    private:
        uint32_t m_RendererID;
        mutable std::unordered_map<std::string, int> m_UniformCache;

        std::string m_vertSource;
        std::string m_fragSource;

    };
}