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

        #define PXL_DEBUG // TODO: Define this in the build system
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
                Logger::LogError("Failed to select any GPU for Vulkan");
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
        m_SwapchainData.Format = m_SurfaceFormat.format;
        m_SwapchainData.ColorSpace = m_SurfaceFormat.colorSpace;

        // TODO: Determine all required swapchain data (surface format, present mode, etc) before creating render pass and swapchain

        // Create default render pass for swapchain framebuffers
        m_DefaultRenderPass = std::make_shared<VulkanRenderPass>(m_Device, m_SurfaceFormat.format); // should get the format of the swapchain not the surface

        // Create swap chain with specified surface
        if (!CreateSwapchain(m_Surface, m_WindowHandle->GetWidth(), m_WindowHandle->GetHeight()))
            return;

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

        // m_ImageAvailableSemaphore = VulkanHelpers::CreateSemaphore(m_Device);
        // m_RenderFinishedSemaphore = VulkanHelpers::CreateSemaphore(m_Device);

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
            Logger::LogError("Failed to allocate command buffer objects");
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
    
    void VulkanContext::SetVSync(bool value)
    {
        // Don't update VSync if it already is the specified value. This is important for vulkan since the entire swapchain needs to be recreated.
        if (m_VSync != value) // lambda possible here I think
        {
            if (value)
                m_SwapchainData.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
            else
                m_SwapchainData.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR; // should be checked under 'available present modes' first

            m_VSync = value;
        }
        else
        {
            return;
        }

        // TODO: recreate swap chain
        // RecreateSwapchain(); // or maybe CreateSwapchain() again but it checks if the swapchain already exists or not.
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
            Logger::LogError("Failed to create Vulkan instance");
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

        Logger::LogInfo("VK Instance Info:");
        Logger::LogInfo("   Application Vulkan API Version: " + apiVersionString);
        Logger::LogInfo("   " + std::to_string(extensions.size()) + " enabled extensions: ");
        
        for (auto& extension : extensions)
        {
            Logger::LogInfo("   - " + std::string(extension));
        }

        Logger::LogInfo("   " + std::to_string(layers.size()) + " enabled layers:");

        for (auto& layer : layers)
        {
            Logger::LogInfo("   - " + std::string(layer));
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

    bool VulkanContext::CreateSwapchain(VkSurfaceKHR surface, uint32_t width, uint32_t height)
    {
        VkResult result;

        // Check surface compatiblity
        auto surfacePresentModes = VulkanHelpers::GetSurfacePresentModes(m_GPU, surface);
        auto surfaceCapabilities = VulkanHelpers::GetSurfaceCapabilities(m_GPU, surface);

        // Select most suitable surface present mode
        bool foundSuitablePresentMode = false;

        for (const auto& presentMode : surfacePresentModes)
        {
            if (m_VSync)
            {
                if (presentMode == VK_PRESENT_MODE_FIFO_KHR)
                {
                    m_SwapchainData.PresentMode = presentMode;
                    foundSuitablePresentMode = true;
                    break; 
                }
            }
            else
            {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    m_SwapchainData.PresentMode = presentMode;
                    foundSuitablePresentMode = true;
                    break;
                }
            }
        }

        if (!foundSuitablePresentMode)
        {
            Logger::LogError("Failed to find suitable swap chain present mode");
            return false;
        }

        // Select most suitable number of images for swapchain
        if (surfaceCapabilities.minImageCount >= 2)
        {
            if (surfaceCapabilities.maxImageCount >= 3)
            {
                m_SwapchainData.ImageCount = 3; // Triple buffering
            }
            else
            {
                m_SwapchainData.ImageCount = 2; // Double buffering
            }
        }
        else
        {
            Logger::LogError("Selected surface for swapchain must support more than 2 images");
            return false;
        }

        // TODO: TEMP
        m_SwapchainData.ImageCount = 3;
        m_SwapchainData.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        // Check if the surface supports the desired swap chain extent
        if (surfaceCapabilities.currentExtent.width == width && surfaceCapabilities.currentExtent.height == height)
        {
            m_SwapchainData.Extent = { width, height };
        }
        else
        {
            Logger::LogError("Selected swap chain size is incompatible with window surface");
            return false;
        }

        // Create swapchain
        m_Swapchain = std::make_shared<VulkanSwapchain>(m_Device, m_Surface, m_SwapchainData, m_DefaultRenderPass);

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