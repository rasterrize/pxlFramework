#include "VulkanShader.h"

#include "Utils/FileSystem.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanShader::VulkanShader(const ShaderSpecs& specs, VkDevice device)
        : Shader(specs), m_Device(device)
    {
        if (specs.FilePath.extension() == ".spv")
        {
            auto code = FileSystem::LoadSPIRV(specs.FilePath);
            m_Module = VulkanUtils::CreateShaderModule(m_Device, code);
        }
    }

    void VulkanShader::Free()
    {
        if (m_Module)
        {
            vkDestroyShaderModule(m_Device, m_Module, nullptr);
            m_Module = VK_NULL_HANDLE;
        }
    }

    void VulkanShader::Reload()
    {
    }

    void VulkanShader::Load()
    {
        if (std::filesystem::exists(m_Specs.FilePath))
        {
            // if (m_Specs)
            //     auto code = FileSystem::LoadSPIRV
            // TODO: reload shader
            // It is then up to the user to call recreate on their pipelines referencing this shader
        }
    }
}