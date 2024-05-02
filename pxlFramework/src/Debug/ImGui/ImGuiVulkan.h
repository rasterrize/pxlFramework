#pragma once

#include "ImGuiBase.h"

#include <vulkan/vulkan.h>

#include "../../Renderer/Vulkan/VulkanContext.h"

namespace pxl
{
    class ImGuiVulkan : public ImGuiBase
    {
    public:
        ImGuiVulkan(const std::shared_ptr<GraphicsContext>& context);

        virtual void NewFrame() override;
        virtual void Render() override;
        virtual void Shutdown() override;

    private:
        std::shared_ptr<VulkanGraphicsContext> m_ContextHandle;
        std::shared_ptr<VulkanDevice> m_Device;
        VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;
    };
}