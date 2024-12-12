#include "VulkanShader.h"

#include <shaderc/shaderc.hpp>

#include "Renderer/Renderer.h"
#include "VulkanHelpers.h"

namespace pxl
{
    VulkanShader::VulkanShader(ShaderStage stage, const std::vector<char>& sprvBin)
        : m_Device(static_cast<VkDevice>(Renderer::GetGraphicsContext()->GetDevice()->GetLogical())), m_ShaderStage(stage)
    {
        m_ShaderModule = CreateShaderModule(m_Device, sprvBin);

        VulkanDeletionQueue::Add([&]()
        {
            Destroy();
        });
    }

    VulkanShader::VulkanShader(ShaderStage stage, const std::string& glslSrc)
        : m_Device(static_cast<VkDevice>(Renderer::GetGraphicsContext()->GetDevice()->GetLogical())), m_ShaderStage(stage)
    {
        // When given glsl source code, compile it to SPIR-V so vulkan can use it
        auto sprvBin = CompileToSPIRV(glslSrc, stage);

        m_ShaderModule = CreateShaderModule(m_Device, sprvBin);

        VulkanDeletionQueue::Add([&]()
        {
            Destroy();
        });
    }

    void VulkanShader::Reload()
    {
    }

    void VulkanShader::Destroy()
    {
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

    VkShaderModule VulkanShader::CreateShaderModule(VkDevice device, const std::vector<uint32_t>& code)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderModuleCreateInfo.codeSize = code.size() * 4;
        shaderModuleCreateInfo.pCode = code.data();

        VkShaderModule shaderModule;
        auto result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create shader module");
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }

    std::vector<uint32_t> VulkanShader::CompileToSPIRV(const std::string& glslSrc, ShaderStage stage)
    {
        // TODO: add SPIRV optimization support

        PXL_PROFILE_SCOPE;

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        shaderc_shader_kind shaderKind = shaderc_shader_kind::shaderc_vertex_shader;

        switch (stage)
        {
            case ShaderStage::Vertex:
                shaderKind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
                break;

            case ShaderStage::Fragment:
                shaderKind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
                break;

            case ShaderStage::Geometry:
                shaderKind = shaderc_shader_kind::shaderc_glsl_geometry_shader;
                break;

            case ShaderStage::Tessellation:
                PXL_LOG_ERROR(LogArea::Vulkan, "Can't select shaderc kind, ShaderStage is tessellation");
                break;
        }

        PXL_LOG_INFO(LogArea::Vulkan, "Compiling GLSL shader to SPIR-V...");

        auto result = compiler.CompileGlslToSpv(glslSrc, shaderKind, "main", options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "shaderc failed to compile glsl to SPIR-V: {}", result.GetErrorMessage());
            return std::vector<uint32_t>();
        }

        PXL_LOG_INFO(LogArea::Vulkan, "Finished compiling SPIR-V")

        return { result.cbegin(), result.cend() };
    }
}