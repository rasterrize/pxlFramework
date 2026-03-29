#include "VulkanShader.h"

#include "VulkanUtils.h"

namespace pxl
{
    VulkanShader::VulkanShader(const ShaderSpecs& specs, VkDevice device)
        : Shader(specs), m_Device(device)
    {
        m_Module = VulkanUtils::CreateShaderModule(m_Device, specs.Code);
    }

    void VulkanShader::Free()
    {
        if (m_Module)
        {
            vkDestroyShaderModule(m_Device, m_Module, nullptr);
            m_Module = VK_NULL_HANDLE;
        }
    }

    bool VulkanShader::Reload(const std::vector<uint32_t>& code)
    {
        Free();

        m_Specs.Code = code;
        m_Module = VulkanUtils::CreateShaderModule(m_Device, m_Specs.Code);
        return m_Module;
    }

}