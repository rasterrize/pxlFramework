#include "VulkanContext.h"

#include "VulkanAllocator.h"
#include "VulkanHelpers.h"
#include "VulkanInstance.h"

namespace pxl
{
    VulkanGraphicsContext::VulkanGraphicsContext(const std::shared_ptr<Window>& window)
    {
        auto vulkanInstance = VulkanInstance::Get();

        // Get the window surface from the window
        m_Surface = window->CreateVKWindowSurface(vulkanInstance);

        VulkanDeletionQueue::Add([&]() {
            vkDestroySurfaceKHR(VulkanInstance::Get(), m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        });

        // Get available physical devices
        auto physicalDevices = VulkanHelpers::GetAvailablePhysicalDevices(vulkanInstance);

        if (physicalDevices.empty())
            return;

        // Select GPU
        VkPhysicalDevice selectedGPU = VK_NULL_HANDLE;

        selectedGPU = GetFirstDiscreteGPU(physicalDevices);
        if (selectedGPU == VK_NULL_HANDLE)
        {
            selectedGPU = physicalDevices[0];

            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(selectedGPU, &properties);
            PXL_LOG_INFO(LogArea::Vulkan, "Selected first non-discrete VK capable GPU: {}", properties.deviceName);
        }

        // Select Queue Families
        auto queueFamilies = VulkanHelpers::GetQueueFamilies(selectedGPU);

        // Graphics contexts require a graphics queue obviously
        auto graphicsQueueFamily = VulkanHelpers::GetSuitableGraphicsQueueFamily(queueFamilies, selectedGPU, m_Surface);

        // Create Logical Device for selected Physical Device
        m_Device = std::make_shared<VulkanDevice>(selectedGPU, graphicsQueueFamily.value());

        PXL_ASSERT_MSG(m_Device, "VulkanGraphicsContext failed to create VulkanDevice object");

        m_Device->LogDeviceLimits();

        auto logicalDevice = static_cast<VkDevice>(m_Device->GetLogical());

        // Create command pool
        VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = graphicsQueueFamily.value();

        VK_CHECK(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &m_CommandPool));

        VulkanDeletionQueue::Add([&]() {
            vkDestroyCommandPool(static_cast<VkDevice>(m_Device->GetLogical()), m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        });

        // Get graphics/present queue from device
        m_GraphicsQueue = VulkanHelpers::GetQueueHandle(logicalDevice, graphicsQueueFamily);
        m_PresentQueue = VulkanHelpers::GetQueueHandle(logicalDevice, graphicsQueueFamily);

        // Get swapchain suitable surface format (for renderpass)
        auto surfaceFormats = VulkanHelpers::GetSurfaceFormats(selectedGPU, m_Surface);
        m_SurfaceFormat = VulkanHelpers::GetSuitableSurfaceFormat(surfaceFormats);

        // Create default render pass for swapchain framebuffers
        m_DefaultRenderPass = std::make_shared<VulkanRenderPass>(logicalDevice, m_SurfaceFormat.format); // should get the format of the swapchain not the surface

        // Create swap chain with specified surface
        VkExtent2D swapchainExtent;
        swapchainExtent.width = window->GetFramebufferSize().Width;
        swapchainExtent.height = window->GetFramebufferSize().Height;
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