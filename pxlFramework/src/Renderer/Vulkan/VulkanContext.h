#pragma once

#include "../GraphicsContext.h"

#include <vulkan/vulkan.h>

#include "../../Core/Window.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanHelpers.h"

namespace pxl
{
    struct VulkanFrame
    {
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
        VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
        VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
        VkFence InFlightFence = VK_NULL_HANDLE;
    };
    
    class VulkanGraphicsContext : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(const std::shared_ptr<Window>& window);
        virtual ~VulkanGraphicsContext() override;

        virtual void Present() override;
        virtual void SetVSync(bool value) override { m_Swapchain->SetVSync(value); m_Swapchain->Recreate(); };
        virtual bool GetVSync() override { return m_Swapchain->GetVSync(); }

        VkInstance GetInstance() const { return m_Instance; }
        std::shared_ptr<VulkanDevice> GetDevice() const { return m_Device; }
        VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }

        std::shared_ptr<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }

        void AcquireNextImage() { m_CurrentImageIndex = m_Swapchain->AcquireNextAvailableImageIndex(m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore); }
        uint32_t GetCurrentFrameIndex() const { return m_CurrentImageIndex; }
        VulkanFrame GetCurrentFrame() const { return m_Frames[m_CurrentFrameIndex]; } // GetNextFrame()?

        VkCommandBuffer CreateCommandBuffer(); // could be a vulkan helper or in the VulkanDevice class
        void SubmitCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkFence signalFence);

        // TEMP (I think)
        std::shared_ptr<VulkanRenderPass> GetDefaultRenderPass() const { return m_DefaultRenderPass; } // Geometry Render Pass?
        
    private:
        void Init();
        void Shutdown();

        bool CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
        //bool CreateLogicalDevice(VkPhysicalDevice physicalDevice);

        VkPhysicalDevice GetFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices);

    private:
        std::shared_ptr<Window> m_WindowHandle = nullptr;

        // Vulkan Handles
        VkInstance m_Instance = VK_NULL_HANDLE;
        std::shared_ptr<VulkanDevice> m_Device;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // TODO: Not sure if these should stay here. They could be in window class but that would put vulkan code in window class
        VkSurfaceFormatKHR m_SurfaceFormat;

        std::shared_ptr<VulkanSwapchain> m_Swapchain;

        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        uint32_t m_CurrentImageIndex = 0;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        // Synchronization
        int m_MaxFramesInFlight = 3; // should used as a queue limit or something idk
        std::vector<VulkanFrame> m_Frames;
        uint32_t m_CurrentFrameIndex = 0;

        // IDK
        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass; // should a default renderpass exist? could this be a geometry renderpass instead?
    };
}