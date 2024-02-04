#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

namespace pxl
{
    class VulkanHelpers
    {
    public:
        static void CheckVkResult(VkResult result); // THIS SHOULD BE A PREPROCESSOR MACRO SO IT CAN BE REMOVED IN RELEASE BUILDS

        static uint32_t GetVulkanAPIVersion();

        static std::vector<VkLayerProperties> GetAvailableInstanceLayers();
        static std::vector<const char*> GetValidationLayers(const std::vector<VkLayerProperties>& availableLayers);

        static std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance);
        static std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice physicalDevice);
        static std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice physicalDevice);

        static std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static std::vector<VkPresentModeKHR> GetSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        static std::optional<uint32_t> GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies, VkPhysicalDevice gpu, VkSurfaceKHR surface);
        static VkSurfaceFormatKHR GetSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
        static VkQueue GetQueueHandle(VkDevice device, const std::optional<uint32_t>& queueIndex);

        static VkSemaphore CreateSemaphore(VkDevice device);
        static VkFence CreateFence(VkDevice device, bool signaled = false);
    };
}