#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Shader.h"
#include "VulkanDevice.h"

namespace pxl
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::shared_ptr<VulkanDevice>& device, ShaderStage stage, const std::vector<char>& sprvBin);
        VulkanShader(const std::shared_ptr<VulkanDevice>& device, ShaderStage stage, const std::string& glslSrc);

        virtual void Reload() override;

        virtual ShaderStage GetShaderStage() const override { return m_ShaderStage; }

        void Destroy();

        VkShaderModule GetShaderModule() const { return m_ShaderModule; }

    private:
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<uint32_t>& code);

        static std::vector<uint32_t> CompileToSPIRV(const std::string& glslSrc, ShaderStage stage);

    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
        ShaderStage m_ShaderStage = ShaderStage::Vertex;
    };
}