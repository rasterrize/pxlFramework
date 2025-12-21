#include "VulkanContext.h"

#include "VulkanAllocator.h"
#include "VulkanHelpers.h"
#include "VulkanInstance.h"

namespace pxl
{
    VulkanGraphicsContext::VulkanGraphicsContext(const std::shared_ptr<Window>& window)
    {
        auto vulkanInstance = VulkanInstance::Get();

        PXL_ASSERT(vulkanInstance);

        // Get the window surface from the window
        glfwCreateWindowSurface(vulkanInstance, window->GetNativeWindow(), nullptr, &m_Surface);

        PXL_ASSERT(m_Surface);

        VulkanDeletionQueue::Add([&]()
        {
            vkDestroySurfaceKHR(VulkanInstance::Get(), m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        });

        // Get available physical devices
        auto physicalDevices = VulkanHelpers::GetAvailablePhysicalDevices(vulkanInstance);

        if (physicalDevices.empty())
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to find any vulkan supported GPU");
            return;
        }

        // Select GPU
        VkPhysicalDevice selectedGPU = VulkanHelpers::GetFirstDiscreteGPU(physicalDevices);

        if (!selectedGPU)
        {
            selectedGPU = physicalDevices[0];

            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(selectedGPU, &properties);
            PXL_LOG_INFO(LogArea::Vulkan, "Selected first non-discrete VK capable GPU: {}", properties.deviceName);
        }

        // Create Logical Device for selected Physical Device
        m_Device = std::make_shared<VulkanDevice>(selectedGPU, m_Surface);

        // Get swapchain suitable surface format (for renderpass)
        auto surfaceFormats = VulkanHelpers::GetSurfaceFormats(selectedGPU, m_Surface);
        m_SurfaceFormat = VulkanHelpers::GetSuitableSurfaceFormat(surfaceFormats);

        // TODO: See what I can do about this
        // Create default render pass for swapchain framebuffers
        m_DefaultRenderPass = std::make_shared<VulkanRenderPass>(m_Device, m_SurfaceFormat.format); // should get the format of the swapchain not the surface

        // Create swap chain
        VkExtent2D swapchainExtent = {};
        swapchainExtent.width = window->GetFramebufferSize().Width;
        swapchainExtent.height = window->GetFramebufferSize().Height;
        m_Swapchain = std::make_shared<VulkanSwapchain>(m_Device, m_Surface, m_SurfaceFormat, swapchainExtent, m_DefaultRenderPass);
    }

    void VulkanGraphicsContext::Present()
    {
        PXL_PROFILE_SCOPE;

        m_Swapchain->QueuePresent();
    }
}