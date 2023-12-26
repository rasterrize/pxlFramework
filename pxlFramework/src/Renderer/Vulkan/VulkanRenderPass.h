#pragma once

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanContext;

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VkDevice device, VkFormat format);
        ~VulkanRenderPass();

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