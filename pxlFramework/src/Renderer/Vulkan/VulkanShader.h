#pragma once

#include <volk/volk.h>

#include "Renderer/Shader.h"

namespace pxl
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const ShaderSpecs& specs, VkDevice device);

        virtual void Free() override;

        virtual bool Reload(const std::vector<uint32_t>& code) override;

        virtual const ShaderSpecs& GetSpecs() const override { return m_Specs; }
        virtual void SetSpecs(const ShaderSpecs& specs) override { m_Specs = specs; }

        VkShaderModule GetModule() const { return m_Module; }

    private:
        ShaderSpecs m_Specs = {};

        VkShaderModule m_Module = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
    };
}