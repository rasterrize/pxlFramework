#include "VulkanContext.h"

#include "VulkanHelpers.h"

// temp
#include "VulkanBuffer.h"

namespace pxl
{
    VulkanContext::VulkanContext(const std::shared_ptr<Window>& window)
        : m_WindowHandle(window)
    {
        Init();
    }

    VulkanContext::~VulkanContext()
    {
        Shutdown();
    }

    void VulkanContext::Init()
    {
        VkResult result;

        // Create Vulkan Instance
        // Get the extensions required for Vulkan to work with GLFW (should retrieve VK_KHR_SURFACE and platform specific extensions (VK_KHR_win32_SURFACE))
        auto glfwExtensions = m_WindowHandle->GetVKRequiredInstanceExtensions();

        // Select from available layers
        auto availableLayers = VulkanHelpers::GetAvailableInstanceLayers();
        auto selectedLayers = std::vector<const char*>();

        #ifdef PXL_DEBUG
            auto validationLayers = VulkanHelpers::GetValidationLayers(availableLayers);
            selectedLayers = validationLayers;
        #endif

        // Create Vulkan Instance with specified extensions and layers
        if (!CreateInstance(glfwExtensions, selectedLayers))
            return;

        // Get the window surface from the window
        m_Surface = m_WindowHandle->CreateVKWindowSurface(m_Instance);

        // Get available physical devices
        auto physicalDevices = VulkanHelpers::GetAvailablePhysicalDevices(m_Instance);

        if (physicalDevices.size() < 1)
            return;
        
        // Select GPU (assigns m_GPU respectively)
        if (!SelectFirstDiscreteGPU(physicalDevices))
        {
            if (!SelectFirstVKCapableGPU(physicalDevices))
            {
                PXL_LOG_ERROR(LogArea::Vulkan, "Failed to select any GPU for Vulkan");
                return;
            }
        }

        // Select Queue Families
        auto queueFamilies = VulkanHelpers::GetQueueFamilies(m_GPU);
        m_GraphicsQueueFamilyIndex = VulkanHelpers::GetSuitableGraphicsQueueFamily(queueFamilies, m_GPU, m_Surface);

        // Create Logical Device for selected Physical Device
        if (!CreateLogicalDevice(m_GPU))
            return;

        // Get present queue (graphics queue) from device
        m_PresentQueue = VulkanHelpers::GetQueueHandle(m_Device, m_GraphicsQueueFamilyIndex);

        // Get swapchain suitable surface format (for renderpass)
        auto surfaceFormats = VulkanHelpers::GetSurfaceFormats(m_GPU, m_Surface);
        auto m_SurfaceFormat = VulkanHelpers::GetSuitableSurfaceFormat(surfaceFormats);

        // TODO: Determine all required swapchain data (surface format, present mode, etc) before creating render pass and swapchain
        

        // Create default render pass for swapchain framebuffers
        m_DefaultRenderPass = std::make_shared<VulkanRenderPass>(m_Device, m_SurfaceFormat.format); // should get the format of the swapchain not the surface

        // Create swap chain with specified surface
        m_Swapchain = std::make_shared<VulkanSwapchain>(m_Device, m_GPU, m_Surface, m_SurfaceFormat, m_WindowHandle, m_DefaultRenderPass);

        // Create command pool
        VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex.value();

        result = vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_CommandPool);
        VulkanHelpers::CheckVkResult(result);

        // Prepare Frames // TODO: This needs to be adjustable at runtime
        m_Frames.resize(m_MaxFramesInFlight);

        for (auto& frame : m_Frames)
        {
            frame.CommandBuffer = CreateCommandBuffer();
            frame.ImageAvailableSemaphore = VulkanHelpers::CreateSemaphore(m_Device);
            frame.RenderFinishedSemaphore = VulkanHelpers::CreateSemaphore(m_Device);
            frame.InFlightFence = VulkanHelpers::CreateFence(m_Device, true);
        }
    }

    void VulkanContext::Shutdown()
    {
        // Wait until device isnt using these objects before deleting them
        vkDeviceWaitIdle(m_Device);
        
        // Destroy VK objects
        for (auto& frame : m_Frames)
        {
            if (frame.ImageAvailableSemaphore != VK_NULL_HANDLE)
                vkDestroySemaphore(m_Device, frame.ImageAvailableSemaphore, nullptr);

            if (frame.RenderFinishedSemaphore != VK_NULL_HANDLE)
                vkDestroySemaphore(m_Device, frame.RenderFinishedSemaphore, nullptr);
        }

        if (m_CommandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

        m_Swapchain->Destroy();

        if (m_Surface != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

        if (m_Device != VK_NULL_HANDLE)
            vkDestroyDevice(m_Device, nullptr);

        if (m_Instance != VK_NULL_HANDLE)
            vkDestroyInstance(m_Instance, nullptr);
    }

    VkCommandBuffer VulkanContext::CreateCommandBuffer()
    {
        // Create command buffer
        VkCommandBuffer commandBuffer;

        VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocInfo.commandPool = m_CommandPool;
        commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocInfo.commandBufferCount = 1;

        auto result = vkAllocateCommandBuffers(m_Device, &commandBufferAllocInfo, &commandBuffer);
        VulkanHelpers::CheckVkResult(result);

        if (result != VK_SUCCESS)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to allocate command buffer objects");
            return VK_NULL_HANDLE;
        }

        return commandBuffer;
    }

    void VulkanContext::SubmitCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkFence signalFence)
    {
        // Submit the command buffer
        VkSubmitInfo commandBufferSubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        
        VkSemaphore waitSemaphores[] = { m_Frames[m_CurrentFrameIndex].ImageAvailableSemaphore }; // The semaphores to wait before execution
        VkSemaphore signalSemaphores[] = { m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore };

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // which stages of the pipeline to wait on
        commandBufferSubmitInfo.waitSemaphoreCount = 1;
        commandBufferSubmitInfo.pWaitSemaphores = waitSemaphores; // semaphores to wait on before execution
        commandBufferSubmitInfo.pWaitDstStageMask = waitStages; // TODO: Understand this a little bit more
        commandBufferSubmitInfo.commandBufferCount = 1;
        commandBufferSubmitInfo.pCommandBuffers = &commandBuffer;
        commandBufferSubmitInfo.signalSemaphoreCount = 1;
        commandBufferSubmitInfo.pSignalSemaphores = signalSemaphores; // semaphores to signal when finished

        vkQueueSubmit(queue, 1, &commandBufferSubmitInfo, signalFence);
    }

    void VulkanContext::Present()
    {
        m_Swapchain->QueuePresent(m_PresentQueue, m_CurrentImageIndex, m_Frames[m_CurrentFrameIndex].RenderFinishedSemaphore);
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
    }

    bool VulkanContext::CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
    {
        VkResult result;

        // Check vulkan API version
        uint32_t apiVersion = VulkanHelpers::GetVulkanAPIVersion();

        // TODO: check if the implementation api version is greater than the specifed application version

        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_3; // The vulkan API version this application (code base) is built on (not implementation version)

        VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = extensions.size();
        instanceInfo.ppEnabledExtensionNames = extensions.data();
        instanceInfo.enabledLayerCount = layers.size();
        instanceInfo.ppEnabledLayerNames = layers.data();

        result = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);
        VulkanHelpers::VulkanHelpers::CheckVkResult(result);

        if (!m_Instance)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create Vulkan instance");
            return false;
        }

        // Logging
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

        return true;
    }

    bool VulkanContext::CreateLogicalDevice(VkPhysicalDevice gpu)
    {
        if (gpu == VK_NULL_HANDLE)
        {
            Logger::LogError("Failed to create logical device, physical device was null pointer");
            return false;
        }

        // Specify Device Queue Create Infos
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        uint32_t queueCount = 0;

        if (m_GraphicsQueueFamilyIndex != -1)
        {
            float queuePriority = 1.0f;
            VkDeviceQueueCreateInfo graphicsQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex.value();
            graphicsQueueCreateInfo.queueCount = 1;
            graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(graphicsQueueCreateInfo);
        }

        // TODO: Specify selected device features
        VkPhysicalDeviceFeatures deviceFeatures = {};
        // GetGPUFeatures()

        std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" }; // necessary for games

        auto availableExtensions = VulkanHelpers::GetDeviceExtensions(m_GPU);

        uint32_t enabledExtensionCount = 0;

        // Check support

        for (const auto& extension : deviceExtensions)
        {
           for (const auto& availableExtension : availableExtensions)
            {
                if (strcmp(extension, availableExtension.extensionName) == 0) 
                {
                    enabledExtensionCount++;
                    break;
                } 
            }
        }

        if (enabledExtensionCount != deviceExtensions.size())
        {
            Logger::LogError("Failed to enable all selected device extensions");
            return false;
        }

        // Specify Device Create Info
        VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());;
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(gpu, &deviceInfo, nullptr, &m_Device);
        VulkanHelpers::CheckVkResult(result);

        if (!m_Device)
        {
            Logger::LogError("Failed to create logical device");
        }
        
        Logger::LogInfo("Logical Device successfully created:");
        Logger::LogInfo("   " + std::to_string(enabledExtensionCount) + " enabled extensions: ");
        
        for (uint32_t i = 0; i < enabledExtensionCount; i++)
        {
            Logger::LogInfo("   - " + std::string(deviceExtensions[i]));
        }
        
        // TODO: expand this

        return true;
    }

    bool VulkanContext::SelectFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices)
    {
        // Find a suitable discrete gpu physical device
        bool foundSuitableDevice = false;
        for (const auto& device : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_GPU = device;
                foundSuitableDevice = true;
                Logger::LogInfo("Selected discrete GPU: " + std::string((const char*)properties.deviceName));
                Logger::LogInfo("- Driver Version: " + std::to_string(properties.driverVersion));
                Logger::LogInfo("- Supported Vulkan API Version: " + std::to_string(properties.apiVersion));
                break;
            }
        }

        if (foundSuitableDevice)
            return true;

        return false;
    }

    bool VulkanContext::SelectFirstVKCapableGPU(const std::vector<VkPhysicalDevice>& physicalDevices)
    {
        m_GPU = physicalDevices[0];

        if (m_GPU = VK_NULL_HANDLE) // no scenario should ever find this null pointer
            return false;

        // Log selected GPU
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[0], &properties);

        Logger::LogInfo("Selected non-discrete VK capable GPU: " + std::to_string((const char)properties.deviceName));
        return true;
    }
}