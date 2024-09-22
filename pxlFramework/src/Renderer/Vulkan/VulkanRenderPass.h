#pragma once

#include <vulkan/vulkan.h>

#include "VulkanDevice.h"

namespace pxl
{
    class VulkanGraphicsContext;

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(const std::shared_ptr<VulkanDevice>& device, VkFormat format);

        void Destroy();

        VkRenderPass GetVKRenderPass() { return m_RenderPass; }

    private:
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;

        std::vector<VkSubpassDescription> m_Subpasses;
        std::vector<VkSubpassDependency> m_SubpassDependencies;

        // for destruction
        VkDevice m_Device = VK_NULL_HANDLE;
    };
}