#pragma once

#include "../Shader.h"

#include <vulkan/vulkan.h>

#include "VulkanDevice.h"

namespace pxl
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::shared_ptr<VulkanDevice>& device, ShaderStage stage, const std::vector<char>& sprvBin);
        ~VulkanShader();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Reload() override;

        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
        virtual void SetUniformInt1(const std::string& name, int value) override;
        virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) override;

        void Destroy();

        VkShaderModule GetShaderModule() { return m_ShaderModule; }
    private:
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
    private:
        VkDevice m_Device;
        VkShaderModule m_ShaderModule;
        ShaderStage m_ShaderStage = ShaderStage::None;
    };
}