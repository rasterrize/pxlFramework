#include "VulkanContext.h"

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h> // should only be included in Debug builds

namespace pxl
{
    void CheckVkResult(VkResult error) // this function shouldnt be here (we need it for every vulkan class)
    {
        if (error == VK_SUCCESS)
		    return;

        Logger::LogError("VkResult wasn't VK_SUCCESS, error code is " + std::to_string(error));

	    if (error < 0)
		    abort(); // probably shouldn't abort immediately
    }

    VulkanContext::VulkanContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
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
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // GLFW shouldn't be here

        // Select from available layers
        auto availableLayers = GetAvailableLayers();
        auto selectedLayers = std::vector<const char*>();

        #define PXL_DEBUG // TODO: Define this in the build system
        #ifdef PXL_DEBUG
            auto validationLayers = GetValidationLayers(availableLayers);
            selectedLayers = validationLayers;
        #endif

        // Create Vulkan Instance with specified extensions and layers
        if (!CreateInstance(glfwExtensionCount, glfwExtensions, selectedLayers))
            return;

        // Create window surface (using m_WindowHandle)
        if (!CreateWindowSurface())
            return;

        // Get available physical devices
        auto physicalDevices = GetAvailablePhysicalDevices();

        if (physicalDevices.size() == 0)
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
        auto queueFamilies = GetQueueFamilies(m_GPU);
        m_GraphicsQueueFamilyIndex = GetSuitableGraphicsQueueFamily(queueFamilies);

        // Create Logical Device for selected Physical Device
        if (!CreateLogicalDevice(m_GPU))
            return;

        // Retrieve the selected queue handles (graphics and present for now)
        if (!RetrieveQueueHandles())
            return;

        // Create swap chain with specified surface
        if (!CreateSwapchain(m_Surface))
            return;

        // TODO: Render passes / Graphics Pipeline / Shaders

    }

    void VulkanContext::Shutdown()
    {
        // Destroy VK objects
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyDevice(m_Device, nullptr); // should these go in the opposite order they were created
        vkDestroyInstance(m_Instance, nullptr);
    }

    void VulkanContext::SwapBuffers()
    {
        // Acquire the next available image in the swapchain so we can d
        uint32_t imageIndex;
        //vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, imageAvailableSemaphore, imageAvailableFence, &imageIndex);
    }
    
    void VulkanContext::SetVSync(bool value)
    {
        // Don't update VSync if it already is the specified value. This is important for vulkan since the entire swapchain needs to be recreated.
        if (m_VSync != value) // lambda possible here I think
        {
            if (value)
                m_SwapchainData.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
            else
                m_SwapchainData.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

            m_VSync = value;
        }
        else
        {
            return;
        }

        // TODO: recreate swap chain
        // RecreateSwapchain(); // or maybe CreateSwapchain() again but it checks if the swapchain already exists or not.
    }

    bool VulkanContext::CreateInstance(uint32_t extensionCount, const char** extensions, const std::vector<const char*>& layers)
    {
        VkResult result;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_3; // should check for available version of vulkan if possible

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = extensionCount;
        instanceInfo.ppEnabledExtensionNames = extensions;
        instanceInfo.enabledLayerCount = layers.size();
        instanceInfo.ppEnabledLayerNames = layers.data();

        result = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);
        CheckVkResult(result);

        if (!m_Instance)
        {
            Logger::LogError("Failed to create Vulkan instance");
            return false;
        }

        // Logging
        std::string apiVersion;
        switch (appInfo.apiVersion)
        {
            case VK_API_VERSION_1_0:
                apiVersion = "1.0";
                break;
            case VK_API_VERSION_1_1:
                apiVersion = "1.1";
                break;
            case VK_API_VERSION_1_2:
                apiVersion = "1.2";
                break;
            case VK_API_VERSION_1_3:
                apiVersion = "1.3";
                break;
                
        }

        Logger::LogInfo("VK Instance Info:");
        Logger::LogInfo("   Vulkan API Version: " + apiVersion);
        Logger::LogInfo("   " + std::to_string(extensionCount) + " enabled extensions: ");
        
        for (uint32_t i = 0; i < extensionCount; i++)
        {
            Logger::LogInfo("   - " + std::string(extensions[i]));
        }

        Logger::LogInfo("   " + std::to_string(layers.size()) + " enabled layers:");

        for (auto layer : layers)
        {
            Logger::LogInfo("   - " + std::string(layer));
        }

        return true;
    }

    bool VulkanContext::CreateLogicalDevice(const VkPhysicalDevice& gpu)
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
            VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
            graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
            graphicsQueueCreateInfo.queueCount = 1;
            graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(graphicsQueueCreateInfo);
        }

        // TODO: Specify selected device features
        VkPhysicalDeviceFeatures deviceFeatures = {};
        // GetGPUFeatures()

        std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" }; // necessary for games

        auto availableExtensions = GetDeviceExtensions(m_GPU);

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
        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());;
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(gpu, &deviceInfo, nullptr, &m_Device);
        CheckVkResult(result);

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

    bool VulkanContext::RetrieveQueueHandles()
    {
        if (m_GraphicsQueueFamilyIndex != -1)
        {
            vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
            vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_PresentQueue); // need to look into how present queues work

            if (!m_GraphicsQueue || !m_PresentQueue)
            {
                Logger::LogError("Failed to retrieve queue handles");
                return false;
            }
            
            return true;
        }

        return false;
    }

    bool VulkanContext::CreateSwapchain(const VkSurfaceKHR& surface)
    {
        VkResult result;

        // Check surface compatiblity
        auto surfaceFormats = GetSurfaceFormats(m_GPU, surface);
        auto surfacePresentModes = GetSurfacePresentModes(m_GPU, surface);
        auto surfaceCapabilities = GetSurfaceCapabilities(m_GPU, surface);

        // Select most suitable surface format
        bool foundSuitableSurfaceFormat = false;
        for (const auto& surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                m_SurfaceFormat = surfaceFormat;
                foundSuitableSurfaceFormat = true;
            }
        }

        if (!foundSuitableSurfaceFormat)
        {
            Logger::LogError("Failed to find suitable surface format for swap chain");
            return false;
        }

        // Select most suitable surface present mode
        bool foundSuitablePresentMode = false;
        for (const auto& presentMode : surfacePresentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                m_SwapchainData.PresentMode = presentMode;
                foundSuitablePresentMode = true;
                break;
            }
        }

        if (!foundSuitablePresentMode)
        {
            Logger::LogError("Failed to find suitable surface present mode for swap chain");
            m_SwapchainData.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
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

        m_SwapchainData.ImageCount = 2; // TEMP: This is just so I can get Vulkan working

        m_SwapchainData.Images.resize(m_SwapchainData.ImageCount);
        m_SwapchainData.ImageViews.resize(m_SwapchainData.ImageCount);
        m_SwapchainData.Framebuffers.resize(m_SwapchainData.ImageCount);

        // Create Swapchain
        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = m_Surface;
        swapchainInfo.minImageCount = m_SwapchainData.ImageCount;
        swapchainInfo.imageFormat = m_SurfaceFormat.format;
        swapchainInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        swapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.presentMode = m_SwapchainData.PresentMode;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: look into this variable
        // TODO: other settings

        result = vkCreateSwapchainKHR(m_Device, &swapchainInfo, nullptr, &m_Swapchain);
        CheckVkResult(result);

        // Get the images from the swapchain
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);

        if (swapchainImageCount != m_SwapchainData.ImageCount)
        {
            Logger::LogError("Swapchain created a different number of images than the number specified");
            return false;
        }

        result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, m_SwapchainData.Images.data());
        CheckVkResult(result);

        // Create Image Views for swapchain images
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = m_SwapchainData.Images[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_SurfaceFormat.format;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // subresourceRange determines how the image should be accessed
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            result = vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapchainData.ImageViews[i]);
            CheckVkResult(result);
        }

        return true;
    }

    bool VulkanContext::CreateWindowSurface()
    {
        // Create VkSurfaceKHR for glfw window
        VkResult result = glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface); // should learn to do this myself https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface
        CheckVkResult(result);

        if (!m_Surface)
        {
            Logger::LogError("Failed to create window surface");
            return false;
        }
        
        return true;
    }

    const std::vector<VkLayerProperties> VulkanContext::GetAvailableLayers()
    {
        // Get layer count
        uint32_t availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

        // Get layers
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        return availableLayers;
    }

    const std::vector<const char*> VulkanContext::GetValidationLayers(const std::vector<VkLayerProperties>& availableLayers)
    {
        const char* validationLayers[] = {
            "VK_LAYER_KHRONOS_validation" // There are more than just this one, we are just using the main one for now
        };
        uint32_t enabledLayerCount = 1;

        std::vector<const char*> selectedValidationLayers;
            
        // Find enabled layers and throw errors if any are not found
        for (const auto& layer : validationLayers) // idk if this should be const auto&
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layer, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                }
            }

            if (layerFound)
            {
                selectedValidationLayers.push_back(layer);
            }
        }

        return selectedValidationLayers;
    }

    const std::vector<VkPhysicalDevice> VulkanContext::GetAvailablePhysicalDevices()
    {
        // Get available physical device count
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            Logger::LogError("Failed to find any vulkan supported GPU");
            return std::vector<VkPhysicalDevice>();
        }

        // Get Vulkan supported physical devices (GPUs)
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        return devices;
    }

    const std::vector<VkQueueFamilyProperties> VulkanContext::GetQueueFamilies(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        if (queueFamilyCount > 0)
        {
            Logger::LogInfo("Found " + std::to_string(queueFamilyCount) + " queue families on physical device");
        }
        else
        {
            Logger::LogError("Found " + std::to_string(queueFamilyCount) + " queue families on physical device");
            return std::vector<VkQueueFamilyProperties>();
        }

        // Retrieve queue families
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            Logger::LogInfo("Queue Family " + std::to_string(i + 1) + ":");
            Logger::LogInfo(" - " + string_VkQueueFlags(queueFamilies[i].queueFlags));
        }

        return queueFamilies;
    }

    const std::vector<VkExtensionProperties> VulkanContext::GetDeviceExtensions(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

        return extensions;
    }

    const std::vector<VkSurfaceFormatKHR> VulkanContext::GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

        return surfaceFormats;
    }


    const std::vector<VkPresentModeKHR> VulkanContext::GetSurfacePresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {   
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

        return presentModes;
    }

    const VkSurfaceCapabilitiesKHR VulkanContext::GetSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {   
        VkSurfaceCapabilitiesKHR capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        
        return capabilities;
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

    uint32_t VulkanContext::GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies)
    {
        bool foundGraphicsQueue = false;
        uint32_t graphicsQueueIndex = -1;

        // Find suitable queue families
        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            // Find suitable graphics queue family
            if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && !foundGraphicsQueue)
            {
                // Check if this graphics queue family supports presentation to a surface
                VkBool32 surfaceSupport;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_GPU, i, m_Surface, &surfaceSupport);

                if (surfaceSupport == VK_TRUE)
                {
                    graphicsQueueIndex = i;
                    foundGraphicsQueue = true;
                    break;
                }
            }
        }

        if (!foundGraphicsQueue)
            Logger::LogError("Failed to find a suitable graphics queue from physical device");
        else
            Logger::LogInfo("Successfully retrieved graphics queue from physical device");

        return graphicsQueueIndex;
        
    }

}