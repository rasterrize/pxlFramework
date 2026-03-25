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

        VkShaderModule GetModule() const { return m_Module; }

    private:
        VkShaderModule m_Module = nullptr;
        VkDevice m_Device = nullptr;
    };
}