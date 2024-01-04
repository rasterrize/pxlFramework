#include "VulkanHelpers.h"

namespace pxl
{
    void VulkanHelpers::CheckVkResult(VkResult result)
    {
        if (result == VK_SUCCESS)
		    return;

        Logger::LogError("VkResult wasn't VK_SUCCESS, error code is " + std::string(string_VkResult(result)));

	    if (result < 0)
		    abort(); // probably shouldn't abort immediately
    }

    uint32_t VulkanHelpers::GetVulkanAPIVersion()
    {
        uint32_t apiVersion;
        VkResult result = vkEnumerateInstanceVersion(&apiVersion);
        VulkanHelpers::CheckVkResult(result);
        return apiVersion;
    }

    std::vector<VkLayerProperties> VulkanHelpers::GetAvailableInstanceLayers()
    {
        // Get layer count
        uint32_t availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

        // Get layers
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        return availableLayers;
    }

    std::vector<const char*> VulkanHelpers::GetValidationLayers(const std::vector<VkLayerProperties>& availableLayers)
    {
        const char* validationLayers[] = {
            "VK_LAYER_KHRONOS_validation" // TODO: implement a system that selects the correct validation layers in a priority
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

    std::vector<VkPhysicalDevice> VulkanHelpers::GetAvailablePhysicalDevices(const VkInstance& instance)
    {
        // Get available physical device count
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            Logger::LogError("Failed to find any vulkan supported GPU");
            return std::vector<VkPhysicalDevice>();
        }

        // Get Vulkan supported physical devices (GPUs)
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        return devices;
    }

    std::vector<VkQueueFamilyProperties> VulkanHelpers::GetQueueFamilies(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        if (queueFamilyCount < 1)
        {
            Logger::LogError("Physical device has no queue families :(");
            return std::vector<VkQueueFamilyProperties>();
        }

        Logger::LogInfo("Found " + std::to_string(queueFamilyCount) + " queue families on physical device");

        // Retrieve queue families
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        // for (uint32_t i = 0; i < queueFamilyCount; i++)
        // {
        //     Logger::LogInfo("Queue Family " + std::to_string(i + 1) + ":");
        //     Logger::LogInfo(" - " + string_VkQueueFlags(queueFamilies[i].queueFlags));
        // }

        return queueFamilies;
    }

    std::vector<VkExtensionProperties> VulkanHelpers::GetDeviceExtensions(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

        return extensions;
    }

    std::vector<VkSurfaceFormatKHR> VulkanHelpers::GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

        return surfaceFormats;
    }

    std::vector<VkPresentModeKHR> VulkanHelpers::GetSurfacePresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {   
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

        return presentModes;
    }

    VkSurfaceCapabilitiesKHR VulkanHelpers::GetSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
    {   
        VkSurfaceCapabilitiesKHR capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        
        return capabilities;
    }

    std::optional<uint32_t> VulkanHelpers::GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies, const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface)
    {
        bool foundGraphicsQueue = false;
        std::optional<uint32_t> graphicsQueueIndex;

        // Find suitable queue families
        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            // Find suitable graphics queue family
            if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && !foundGraphicsQueue)
            {
                // Check if this graphics queue family supports presentation to a surface
                VkBool32 surfaceSupport;
                vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &surfaceSupport);

                if (surfaceSupport == VK_TRUE)
                {
                    graphicsQueueIndex = i;
                    foundGraphicsQueue = true;
                    break;
                }
            }
        }

        if (!foundGraphicsQueue)
            Logger::LogError("Failed to find a suitable graphics queue family from physical device");
        else
            Logger::LogInfo("Successfully retrieved graphics queue family from physical device");

        return graphicsQueueIndex;
    }

    VkSurfaceFormatKHR VulkanHelpers::GetSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats)
    {
        // Select most suitable surface format
        for (const auto& surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return surfaceFormat;
        }

        Logger::LogError("Failed to find suitable surface format for swap chain");
        return { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR };
    }

    VkQueue VulkanHelpers::GetQueueHandle(VkDevice device, const std::optional<uint32_t>& queueIndex)
    {
        VkQueue queue = VK_NULL_HANDLE;
        if (queueIndex.has_value())
        {
            vkGetDeviceQueue(device, queueIndex.value(), 0, &queue);

            if (!queue)
            {
                Logger::LogError("Failed to retrieve queue handles");
                return VK_NULL_HANDLE;
            }
        }

        return queue;
    }

    VkSemaphore VulkanHelpers::CreateSemaphore(VkDevice device)
    {
        VkSemaphore semaphore;

        VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        // TODO: flags

        auto result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
        
        if (semaphore == VK_NULL_HANDLE)
        {
            Logger::LogError("Failed to create Vulkan semaphore");
            return VK_NULL_HANDLE;
        }
        
        return semaphore;
    }

    VkFence VulkanHelpers::CreateFence(VkDevice device, bool signaled)
    {
        VkFence fence;

        VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

        if (signaled)
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        auto result = vkCreateFence(device, &fenceInfo, nullptr, &fence);
        CheckVkResult(result);

        if (fence == VK_NULL_HANDLE)
        {
            Logger::LogError("Failed to create Vulkan fence");
            return VK_NULL_HANDLE;
        }

        return fence;
    }
}