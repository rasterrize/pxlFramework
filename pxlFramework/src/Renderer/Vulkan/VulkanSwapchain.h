#pragma once

#include <vulkan/vulkan.h>

#include "VulkanImage.h"
#include "VulkanFramebuffer.h"

#include "../../Core/Window.h"

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

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(VkDevice device, VkPhysicalDevice gpu, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, const std::shared_ptr<Window> windowHandle, const std::shared_ptr<VulkanRenderPass>& renderPass);
        ~VulkanSwapchain();

        void Recreate();
        void Destroy();

        uint32_t AcquireNextAvailableImageIndex(VkSemaphore signalSemaphore);

        void QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore/* = VK_NULL_HANDLE*/);

        VkSwapchainKHR GetVKSwapchain() const { return m_Swapchain; }
        VulkanSwapchainSpecs GetSwapchainSpecs() const { return m_SwapchainSpecs; }
        VkImageView GetImageView(uint32_t index) const { return m_Images[index]->GetImageView(); }
        std::shared_ptr<VulkanFramebuffer> GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; }

        void SetVSync(bool value) { m_VSync = value; }
        bool GetVSync() const { return m_VSync; }
    private:
        void Create();
        void PrepareImages();
        void PrepareFramebuffers(const std::shared_ptr<VulkanRenderPass>& renderPass);

        VkPresentModeKHR GetSuitablePresentMode();
        uint32_t GetSuitableImageCount();
        bool CheckExtentSupport(VkExtent2D extent);
    
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;

        std::shared_ptr<Window> m_WindowHandle;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        std::vector<std::shared_ptr<VulkanImage>> m_Images; // holds the image views
        std::vector<std::shared_ptr<VulkanFramebuffer>> m_Framebuffers;

        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass; // to create framebuffers

        VulkanSwapchainSpecs m_SwapchainSpecs = {};
        bool m_VSync = true;
    };
}