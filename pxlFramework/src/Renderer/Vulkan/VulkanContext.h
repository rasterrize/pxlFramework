#pragma once

#include "../GraphicsContext.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(GLFWwindow* windowHandle);
        ~VulkanContext();

        virtual void SwapBuffers() override;
        virtual void SetVSync(bool value) override; // not sure whether to call the parameter 'vsync' or 'value'
        virtual bool GetVSync() override { return m_VSync; }
        
    private:
        virtual void Init() override;
        void Shutdown();

        bool CreateInstance(uint32_t extensionCount, const char** extensions, const std::vector<const char*>& layers);
        bool CreateWindowSurface(); // The GLFWwindow is supplied in the constructor
        bool CreateLogicalDevice(const VkPhysicalDevice& physicalDevice);
        bool CreateSwapchain(const VkSurfaceKHR& surface);

        bool SelectFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices);
        bool SelectFirstVKCapableGPU(const std::vector<VkPhysicalDevice>& physicalDevices);

        bool RetrieveQueueHandles(); // Retrieves the queue handles for all selected queues

        const std::vector<VkLayerProperties> GetAvailableLayers();
        const std::vector<const char*> GetValidationLayers(const std::vector<VkLayerProperties>& availableLayers);

        const std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices();

        const std::vector<VkQueueFamilyProperties> GetQueueFamilies(const VkPhysicalDevice& physicalDevice);
        const std::vector<VkExtensionProperties> GetDeviceExtensions(const VkPhysicalDevice& physicalDevice);

        const std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
        const std::vector<VkPresentModeKHR> GetSurfacePresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
        const VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);

        uint32_t GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies);
    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;

        // Vulkan Handles
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // TODO: I think this should be in the Window/WindowGLFW class, but idk
        VkSurfaceFormatKHR m_SurfaceFormat;
        uint32_t m_GraphicsQueueFamilyIndex = -1; // This should use std::optional instead of assigning it to -1

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        struct SwapchainData
        {
            uint32_t ImageCount = 2;
            VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR; // This is guaranteed to be supported
            std::vector<VkImage> Images;
            std::vector<VkImageView> ImageViews;
            std::vector<VkFramebuffer> Framebuffers;
        };

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        SwapchainData m_SwapchainData;
    };
}