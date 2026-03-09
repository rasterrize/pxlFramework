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

        virtual void Reload() override;

        VkShaderModule GetModule() const { return m_Module; }

    private:
        void Load();

    private:
        VkShaderModule m_Module = nullptr;
        VkDevice m_Device = nullptr;
    };
}