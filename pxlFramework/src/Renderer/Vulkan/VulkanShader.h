#pragma once

#include "../Shader.h"

namespace pxl
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader();
        ~VulkanShader();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Reload() override;

        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformInt1(const std::string& name, int value) override;
    private:

    };
}