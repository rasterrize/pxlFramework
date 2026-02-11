#pragma once

#include <volk/volk.h>

#include "GUIBase.h"
#include "Renderer/Vulkan/VulkanContext.h"

namespace pxl
{
    class GUIVulkan : public GUIBase
    {
    public:
        GUIVulkan(const std::shared_ptr<GraphicsContext>& context);

        virtual void NewFrame() override;
        virtual void Render() override;
        virtual void Shutdown() override;

    private:
        std::shared_ptr<VulkanGraphicsContext> m_ContextHandle = nullptr;
        std::shared_ptr<VulkanDevice> m_Device = nullptr;
        VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;
    };
}