#include "VulkanShader.h"

namespace pxl
{
    VulkanShader::VulkanShader(const std::shared_ptr<VulkanDevice>& device, ShaderStage stage, const std::vector<char>& sprvBin)
        : m_Device(static_cast<VkDevice>(device->GetDevice())), m_ShaderStage(stage)
    {
        m_ShaderModule = CreateShaderModule(m_Device, sprvBin);

        VulkanDeletionQueue::Add([&]() {
            Destroy();
        });
    }

    VulkanShader::~VulkanShader()
    {
        Destroy();
    }

    void VulkanShader::Bind()
    {
    }

    void VulkanShader::Unbind()
    {
    }

    void VulkanShader::Reload()
    {
    }

    void VulkanShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
    {
    }

    void VulkanShader::SetUniformInt1(const std::string& name, int value)
    {
    }

    void VulkanShader::Destroy()
    {
        // Destroy shader module
        if (m_ShaderModule)
        {
            vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
            m_ShaderModule = VK_NULL_HANDLE;
        }
    }

    VkShaderModule VulkanShader::CreateShaderModule(VkDevice device, const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderModuleCreateInfo.codeSize = code.size();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // because pCode takes in a uint32_t pointer // need to learn about reinterpret_casts

        VkShaderModule shaderModule;
        auto result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create shader module");
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }
}