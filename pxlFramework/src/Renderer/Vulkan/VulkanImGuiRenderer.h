#pragma once

#include <volk/volk.h>

#include "Core/Window.h"
#include "Renderer/ImGuiRenderer.h"

namespace pxl
{
    class VulkanImGuiRenderer : public ImGuiRenderer
    {
    public:
        VulkanImGuiRenderer(const ImGuiSpecs& specs, VkInstance instance, VkPhysicalDevice gpu, VkDevice device, VkQueue queue, VkFormat format, uint32_t imageCount);

        virtual void Free() override;

        virtual void NewFrame() override;

        virtual void Render(const std::unique_ptr<GraphicsDevice>& device) override;

    private:
        VkDescriptorPool m_Pool = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;
    };
}