#include "VulkanShader.h"

namespace pxl
{
    VulkanShader::VulkanShader(const std::shared_ptr<VulkanDevice>& device, const std::vector<char>& vertBin, const std::vector<char>& fragBin)
        : m_Device(device)
    {
        auto logicalDevice = m_Device->GetVkDevice();

        auto vertModule = CreateShaderModule(logicalDevice, vertBin);
        auto fragModule = CreateShaderModule(logicalDevice, fragBin);

        m_ShaderModules[VK_SHADER_STAGE_VERTEX_BIT] = vertModule;
        m_ShaderModules[VK_SHADER_STAGE_FRAGMENT_BIT] = fragModule;
    }

    VulkanShader::~VulkanShader()
    {
        for (const auto& pair : m_ShaderModules)
            vkDestroyShaderModule(m_Device->GetVkDevice(), pair.second, nullptr);
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

    VkShaderModule VulkanShader::CreateShaderModule(VkDevice device, const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo vertShaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        vertShaderModuleCreateInfo.codeSize = code.size();
        vertShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // because pCode takes in a uint32_t pointer // need to learn about reinterpret_casts

        VkShaderModule shaderModule;
        auto result = vkCreateShaderModule(device, &vertShaderModuleCreateInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create shader module");
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }
}