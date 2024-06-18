#include "VulkanContext.h"

#include "VulkanHelpers.h"
#include "VulkanAllocator.h"
#include "VulkanInstance.h"

namespace pxl
{
    VulkanGraphicsContext::VulkanGraphicsContext(const std::shared_ptr<Window>& window)
    {
        // Get the extensions required for Vulkan to work with GLFW (should retrieve VK_KHR_SURFACE and platform specific extensions (VK_KHR_win32_SURFACE))
        auto glfwExtensions = Window::GetVKRequiredInstanceExtensions();

        // Select from available layers
        auto availableLayers = VulkanHelpers::GetAvailableInstanceLayers();
        auto selectedLayers = std::vector<const char*>();

        #ifdef PXL_DEBUG_VULKAN
            auto validationLayers = VulkanHelpers::GetValidationLayers(availableLayers);
            selectedLayers = validationLayers;
        #endif

        // Create Vulkan instance with specified extensions and layers
        if (!VulkanInstance::Get())
            VulkanInstance::Init(glfwExtensions, selectedLayers);

        auto vulkanInstance = VulkanInstance::Get();

        // Get the window surface from the window
        m_Surface = window->CreateVKWindowSurface(vulkanInstance);

        VulkanDeletionQueue::Add([&]() {
            vkDestroySurfaceKHR(VulkanInstance::Get(), m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        });

        // Get available physical devices
        auto physicalDevices = VulkanHelpers::GetAvailablePhysicalDevices(vulkanInstance);

        if (physicalDevices.size() < 1)
            return;
        
        // Select GPU
        VkPhysicalDevice selectedGPU = VK_NULL_HANDLE;

        selectedGPU = GetFirstDiscreteGPU(physicalDevices);
        if (selectedGPU == VK_NULL_HANDLE)
        {
            selectedGPU = physicalDevices[0];
            if (selectedGPU == VK_NULL_HANDLE)
            {
                PXL_LOG_ERROR(LogArea::Vulkan, "Failed to select any GPU for Vulkan");
                return;
            }
            else
            {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(selectedGPU, &properties);
                PXL_LOG_INFO(LogArea::Vulkan, "Selected first non-discrete VK capable GPU: {}", properties.deviceName);
            }
        }

        // Select Queue Families
        auto queueFamilies = VulkanHelpers::GetQueueFamilies(selectedGPU);

        // Graphics contexts require a graphics queue obviously
        auto graphicsQueueFamily = VulkanHelpers::GetSuitableGraphicsQueueFamily(queueFamilies, selectedGPU, m_Surface);

        // Create Logical Device for selected Physical Device
        m_Device = std::make_shared<VulkanDevice>(selectedGPU, graphicsQueueFamily.value());

        m_Device->LogDeviceLimits();

        if (!m_Device)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "VulkanGraphicsContext failed to create VulkanDevice object");
            return;
        }

        auto logicalDevice = static_cast<VkDevice>(m_Device->GetDevice());

        // Create command pool
        VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = graphicsQueueFamily.value();

        VK_CHECK(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &m_CommandPool));

        VulkanDeletionQueue::Add([&]() {
            vkDestroyCommandPool(static_cast<VkDevice>(m_Device->GetDevice()), m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        });

        // Get present queue (graphics queue) from device
        m_PresentQueue = VulkanHelpers::GetQueueHandle(logicalDevice, graphicsQueueFamily);

        // Get swapchain suitable surface format (for renderpass)
        auto surfaceFormats = VulkanHelpers::GetSurfaceFormats(selectedGPU, m_Surface);
        m_SurfaceFormat = VulkanHelpers::GetSuitableSurfaceFormat(surfaceFormats);

        // Create default render pass for swapchain framebuffers
        m_DefaultRenderPass = std::make_shared<VulkanRenderPass>(logicalDevice, m_SurfaceFormat.format); // should get the format of the swapchain not the surface

        // Create swap chain with specified surface
        VkExtent2D swapchainExtent;
        swapchainExtent.width = window->GetFramebufferSize().x;
        swapchainExtent.height = window->GetFramebufferSize().y;
        m_Swapchain = std::make_shared<VulkanSwapchain>(m_Device, m_Surface, m_SurfaceFormat, swapchainExtent, m_DefaultRenderPass, m_CommandPool);

        // Initialise Vulkan Memory Allocator
        if (!VulkanAllocator::Get())
            VulkanAllocator::Init(vulkanInstance, m_Device);
    }

    void VulkanGraphicsContext::Present()
    {
        PXL_PROFILE_SCOPE;
        
        m_Swapchain->QueuePresent(m_PresentQueue);
    }

    void VulkanGraphicsContext::SubmitCommandBuffer(const VkSubmitInfo& submitInfo, VkQueue queue, VkFence signalFence)
    {
        PXL_PROFILE_SCOPE;
        
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, signalFence));
    }

    VkPhysicalDevice VulkanGraphicsContext::GetFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices)
    {
        // Find a suitable discrete gpu physical device from the given physical devices
        for (const auto& gpu : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(gpu, &properties);
            
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return gpu;
        }

        return VK_NULL_HANDLE;
    }
}