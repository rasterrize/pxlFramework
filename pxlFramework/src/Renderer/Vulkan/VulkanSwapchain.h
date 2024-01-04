#pragma once

#include <vulkan/vulkan.h>

#include "VulkanImage.h"
#include "VulkanFramebuffer.h"

namespace pxl
{
    struct VulkanSwapchainData
    {
        uint32_t ImageCount = 0;
        VkExtent2D Extent = {};
        VkFormat Format = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR; // This is guaranteed to be supported
    };

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(VkDevice device, VkSurfaceKHR surface, const VulkanSwapchainData& swapchainData, const std::shared_ptr<VulkanRenderPass>& renderPass);
        ~VulkanSwapchain();

        void Recreate();
        void Destroy();

        uint32_t AcquireNextAvailableImageIndex(VkSemaphore signalSemaphore);

        void QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore/* = VK_NULL_HANDLE*/);

        VkSwapchainKHR GetVKSwapchain() const { return m_Swapchain; }
        VulkanSwapchainData GetSwapchainData() const { return m_Data; }
        VkImageView GetImageView(uint32_t index) const { return m_Images[index]->GetImageView(); }
        std::shared_ptr<VulkanFramebuffer> GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; }
    private:
        void Create();
        void PrepareImages();
        void PrepareFramebuffers(const std::shared_ptr<VulkanRenderPass>& renderPass);
    
    private:
        VkDevice m_Device = VK_NULL_HANDLE;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        std::vector<std::shared_ptr<VulkanImage>> m_Images; // holds the image views
        std::vector<std::shared_ptr<VulkanFramebuffer>> m_Framebuffers;

        VulkanSwapchainData m_Data = {};
    };
}