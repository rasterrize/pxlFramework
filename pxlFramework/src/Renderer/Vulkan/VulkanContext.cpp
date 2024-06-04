#include "VulkanContext.h"

#include "VulkanHelpers.h"
#include "VulkanAllocator.h"

// temp
#include "../../Core/Stopwatch.h"

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

        // Create Vulkan Instance with specified extensions and layers
        if (!CreateInstance(glfwExtensions, selectedLayers))
            return;

        // Get the window surface from the window
        m_Surface = window->CreateVKWindowSurface(m_Instance);

        VulkanDeletionQueue::Add([&]() {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        });

        // Get available physical devices
        auto physicalDevices = VulkanHelpers::GetAvailablePhysicalDevices(m_Instance);

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
        if (!VulkanAllocator::IsInitialised())
            VulkanAllocator::Init(m_Instance, m_Device);
    }

    VulkanGraphicsContext::~VulkanGraphicsContext()
    {
    }

    void VulkanGraphicsContext::Present()
    {
        PXL_PROFILE_SCOPE;
        
        m_Swapchain->QueuePresent(m_PresentQueue);
    }

    void VulkanGraphicsContext::SubmitCommandBuffer(const VkSubmitInfo& submitInfo, VkQueue queue, VkFence signalFence)
    {
        PXL_PROFILE_SCOPE;
        
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, signalFence)); // TODO: include support for no signal fence?
    }

    bool VulkanGraphicsContext::CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
    {
        // Check vulkan API version
        uint32_t apiVersion = VulkanHelpers::GetVulkanAPIVersion();

        // TODO: check if the implementation api version is greater than the specifed application version

        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_3; // The vulkan API version this application (code base) is built on (not implementation version)

        VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceInfo.ppEnabledExtensionNames = extensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        instanceInfo.ppEnabledLayerNames = layers.data();

        VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &m_Instance));

        if (!m_Instance)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create Vulkan instance");
            return false;
        }

        VulkanDeletionQueue::Add([&]() {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        });

        // Logging 
        #ifdef PXL_ENABLE_LOGGING
        
        std::string apiVersionString;
        switch (appInfo.apiVersion)
        {
            case VK_API_VERSION_1_0:
                apiVersionString = "1.0";
                break;
            case VK_API_VERSION_1_1:
                apiVersionString = "1.1";
                break;
            case VK_API_VERSION_1_2:
                apiVersionString = "1.2";
                break;
            case VK_API_VERSION_1_3:
                apiVersionString = "1.3";
                break;
        }

        PXL_LOG_INFO(LogArea::Vulkan, "VK Instance Info:");
        PXL_LOG_INFO(LogArea::Vulkan, "   Application Vulkan API Version: {}", apiVersionString);

        PXL_LOG_INFO(LogArea::Vulkan, "   {} enabled extensions: ", extensions.size());
        for (auto& extension : extensions)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "   - {}", extension);
        }

        PXL_LOG_INFO(LogArea::Vulkan, "   {} enabled layers:", layers.size());
        for (auto& layer : layers)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "   - {}", layer);
        }

        #endif

        return true;
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