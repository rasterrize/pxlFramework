#pragma once

#include <vulkan/vulkan.h>

#include "VulkanImage.h"
#include "VulkanFramebuffer.h"
#include "VulkanDevice.h"

namespace pxl
{
    struct VulkanSwapchainSpecs
    {
        VkExtent2D Extent = { UINT32_MAX, UINT32_MAX };
        VkFormat Format = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR; // This is guaranteed to be supported
        uint32_t ImageCount = 0;
    };

    struct VulkanFrame
    {
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
        VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
        VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
        VkFence InFlightFence = VK_NULL_HANDLE;
    };

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(const std::shared_ptr<VulkanDevice>& device, VkSurfaceKHR surface, const VkSurfaceFormatKHR& surfaceFormat, const VkExtent2D& imageExtent, const std::shared_ptr<VulkanRenderPass>& renderPass, VkCommandPool commandPool);
        ~VulkanSwapchain();

        void Recreate(uint32_t width, uint32_t height);
        void Recreate();

        void Destroy();
        void DestroyFrameData();

        void AcquireNextAvailableImageIndex();
        uint32_t GetCurrentImageIndex() const { return m_CurrentImageIndex; }

        void QueuePresent(VkQueue queue);

        VkSwapchainKHR GetVKSwapchain() const { return m_Swapchain; }
        VulkanSwapchainSpecs GetSwapchainSpecs() const { return m_SwapchainSpecs; }
        VkImageView GetImageView(uint32_t index) const { return m_Images[index]->GetImageView(); } // Get Current Frame Image?
        std::shared_ptr<VulkanFramebuffer> GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; } // Get Current Frame Framebuffer?
        VulkanFrame GetCurrentFrame() const { return m_Frames[m_CurrentFrameIndex]; }

        void SetVSync(bool value) { m_VSync = value; }
        bool GetVSync() const { return m_VSync; }

        void SetExtent(VkExtent2D extent) { m_SwapchainSpecs.Extent = extent; }

        void Suspend() { m_Suspend = true; }
        void Continue() { m_Suspend = false; }
    private:
        void CreateSwapchain();
        void PrepareImages();
        void PrepareFramebuffers(const std::shared_ptr<VulkanRenderPass>& renderPass);

        VkPresentModeKHR GetSuitablePresentMode();
        uint32_t GetSuitableImageCount();
        bool CheckExtentSupport(VkExtent2D extent);
    
    private:
        const std::shared_ptr<VulkanDevice> m_Device;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        uint32_t m_CurrentImageIndex = 0; // for VkSwapchain images
        std::vector<std::shared_ptr<VulkanImage>> m_Images; // holds the image views
        std::vector<std::shared_ptr<VulkanFramebuffer>> m_Framebuffers;

        // Synchronization
        uint32_t m_MaxFramesInFlight = 3; // should this always match swapchain image count?
        uint32_t m_CurrentFrameIndex = 0; // for CPU side frame data
        std::vector<VulkanFrame> m_Frames;

        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass; // to create framebuffers

        VulkanSwapchainSpecs m_SwapchainSpecs = {};

        bool m_VSync = true;
        bool m_Suspend = false;
    };
}