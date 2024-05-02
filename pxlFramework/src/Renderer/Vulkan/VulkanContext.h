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
    class VulkanGraphicsContext : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(const std::shared_ptr<Window>& window);
        virtual ~VulkanGraphicsContext() override;

        virtual void Present() override;

        virtual void SetVSync(bool value) override { m_Swapchain->SetVSync(value); m_Swapchain->Recreate(); }
        virtual bool GetVSync() override { return m_Swapchain->GetVSync(); }

        virtual std::shared_ptr<Device> GetDevice() override { return m_Device; }

        VkInstance GetInstance() const { return m_Instance; }
        //std::shared_ptr<VulkanDevice> GetDevice() const { return m_Device; }
        VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }
        std::shared_ptr<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }

        VkCommandPool GetCommandPool() const { return m_CommandPool; }

        void SubmitCommandBuffer(const VkSubmitInfo& submitInfo, VkQueue queue, VkFence signalFence); // TODO: move this elsewhere or possibly nuke it

        // TEMP (I think)
        std::shared_ptr<VulkanRenderPass> GetDefaultRenderPass() const { return m_DefaultRenderPass; } // Geometry Render Pass?
        
    private:
        void Init(const std::shared_ptr<Window>& window);
        void Shutdown();

        bool CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers); // Should this class be creating an instance if that instance should be used by a compute context as well

        VkPhysicalDevice GetFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices); // TODO: put this in vulkan helpers
    private:
        // Vulkan Handles
        VkInstance m_Instance = VK_NULL_HANDLE;
        std::shared_ptr<VulkanDevice> m_Device;
        std::shared_ptr<VulkanSwapchain> m_Swapchain;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // TODO: Not sure if these should stay here. They could be in window class but that would put vulkan code in window class
        VkSurfaceFormatKHR m_SurfaceFormat = {};

        //VkQueue m_GraphicsQueue // it is a graphics context after all
        VkQueue m_PresentQueue = VK_NULL_HANDLE; // TODO: either create swapchain with this or have the swapchain get it. I dont think it should be here

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        // IDK
        std::shared_ptr<VulkanRenderPass> m_DefaultRenderPass; // should a default renderpass exist? could this be a geometry renderpass instead? // also, this should probably be apart of the renderer?
    };
}