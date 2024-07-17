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

        virtual void Reload() override;

        virtual ShaderStage GetShaderStage() const override { return m_ShaderStage; }

        void Destroy();

        VkShaderModule GetShaderModule() const { return m_ShaderModule; }
    private:
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
    private:
        VkDevice m_Device;
        VkShaderModule m_ShaderModule;
        ShaderStage m_ShaderStage = ShaderStage::None;
    };
}