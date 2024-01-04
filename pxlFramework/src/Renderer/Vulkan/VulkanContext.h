#pragma once

#include "../GraphicsContext.h"

#include "../../Core/Window.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanSwapchain.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(const std::shared_ptr<Window>& window);
        ~VulkanContext();

        virtual void Present() override;
        virtual void SetVSync(bool value) override; // not sure whether to call the parameter 'vsync' or 'value'
        virtual bool GetVSync() override { return m_VSync; }

        VkInstance GetInstance() const { return m_Instance; }
        VkDevice GetDevice() const { return m_Device; }
        VkPhysicalDevice GetPhysicalDevice() { return m_GPU; }
        uint32_t GetGraphicsQueueIndex() const { return m_GraphicsQueueFamilyIndex.value(); }
        VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }

        std::shared_ptr<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }

        void PrepareNextFrame() { m_CurrentImageIndex = m_Swapchain->AcquireNextAvailableImageIndex(m_ImageAvailableSemaphore); }
        uint32_t GetCurrentFrameIndex() const { return m_CurrentImageIndex; }

        void SubmitCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkFence signalFence);

        // TEMP (I think)
        std::shared_ptr<VulkanRenderPass> GetDefaultRenderPass() const { return m_DefaultRenderPass; }
        
    private:
        void Init();
        void Shutdown();

        bool CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
        bool CreateLogicalDevice(VkPhysicalDevice physicalDevice);
        bool CreateSwapchain(VkSurfaceKHR surface, uint32_t width, uint32_t height);

        bool SelectFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices);
        bool SelectFirstVKCapableGPU(const std::vector<VkPhysicalDevice>& physicalDevices);

    private:
        bool m_VSync = true;
        std::shared_ptr<Window> m_WindowHandle = nullptr;

        // Vulkan Handles
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // TODO: Surfaces and surface creation should be in the window class
        VkSurfaceFormatKHR m_SurfaceFormat;

        std::shared_ptr<VulkanSwapchain> m_Swapchain;
        VulkanSwapchainData m_SwapchainData = {};
        
        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;

        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        uint32_t m_CurrentImageIndex;

        //std::vector<VkCommandBuffer> m_CommandBuffers;

        // Synchronization
        VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
        VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;

        // IDK
        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass;

        bool m_PresentReady = false;
    };
}