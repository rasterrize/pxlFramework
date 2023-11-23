#pragma once

#include "../GraphicsContext.h"

#include "../../Core/Window.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"

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

        VkSurfaceFormatKHR GetSurfaceFormat() const { return m_SurfaceFormat; }
        
    private:
        void Init();
        void Shutdown();

        bool CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
        bool CreateLogicalDevice(const VkPhysicalDevice& physicalDevice);
        bool CreateSwapchain(const VkSurfaceKHR& surface);
        bool CreateFramebuffers(const VkRenderPass& renderPass);
        bool CreateSyncObjects();

        void RecordCommands(uint32_t imageIndex);

        bool SelectFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices);
        bool SelectFirstVKCapableGPU(const std::vector<VkPhysicalDevice>& physicalDevices);

        bool RetrieveQueueHandles(); // Retrieves the queue handles for all selected queues // TODO: Confusing function name

        // TODO: most the functions below feel like helper functions, maybe they should be put into another file
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
        struct SwapchainData
        {
            uint32_t ImageCount = 0;
            VkExtent2D Extent = {};
            VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR; // This is guaranteed to be supported
            std::vector<VkImage> Images;
            std::vector<VkImageView> ImageViews;
            std::vector<VkFramebuffer> Framebuffers;
        };
    private:
        bool m_VSync = true;
        std::shared_ptr<Window> m_WindowHandle = nullptr;

        // Vulkan Handles
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // TODO: Surfaces and surface creation should be in the window class
        VkSurfaceFormatKHR m_SurfaceFormat;
        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        SwapchainData m_SwapchainData = {};

        // Commands and Synchorization
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;
        VkSemaphore m_ImageAvailableSemaphore;
        VkSemaphore m_RenderFinishedSemaphore;
        VkFence m_InFlightFence;

        // Initial testing
        std::shared_ptr<VulkanShader> m_Shader;
        std::shared_ptr<VulkanGraphicsPipeline> m_Pipeline;
        std::shared_ptr<VulkanRenderPass> m_Renderpass;
        VkClearValue m_ClearColour = { { { 20.0f / 255.0f, 24.0f / 255.0f, 28.0f / 255.0f, 1.0f } } };
    };
}