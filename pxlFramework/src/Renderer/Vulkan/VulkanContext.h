#pragma once

#include "Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>

#include "Core/Window.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanHelpers.h"

namespace pxl
{
    class VulkanGraphicsContext : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(const std::shared_ptr<Window>& window);
        virtual ~VulkanGraphicsContext() override = default;

        virtual void Present() override;

        virtual void SetVSync(bool value) override { m_Swapchain->SetVSync(value); m_Swapchain->Recreate(); }
        virtual bool GetVSync() const override { return m_Swapchain->GetVSync(); }

        virtual std::shared_ptr<GraphicsDevice> GetDevice() const override { return m_Device; }

        VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }
        std::shared_ptr<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }

        VkCommandPool GetCommandPool() const { return m_CommandPool; }

        VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; } // move this to device class?

        void SubmitCommandBuffer(const VkSubmitInfo& submitInfo, VkQueue queue, VkFence signalFence = nullptr); // TODO: move this to the device class

        // TEMP (I think)
        std::shared_ptr<VulkanRenderPass> GetDefaultRenderPass() const { return m_DefaultRenderPass; } // Geometry Render Pass?

    private:
        VkPhysicalDevice GetFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices); // TODO: put this in vulkan helpers
    private:
        std::shared_ptr<VulkanDevice> m_Device;
        std::shared_ptr<VulkanSwapchain> m_Swapchain;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // TODO: Not sure if these should stay here. They could be in window class but that would put vulkan code in window class
        VkSurfaceFormatKHR m_SurfaceFormat = {};

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE; // TODO: either create swapchain with this or have the swapchain get it. I dont think it should be here
        VkQueue m_PresentQueue = VK_NULL_HANDLE; // TODO: either create swapchain with this or have the swapchain get it. I dont think it should be here

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        // IDK
        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass; // should a default renderpass exist? could this be a geometry renderpass instead? // also, this should probably be apart of the renderer?
    };
}