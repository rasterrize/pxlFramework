#pragma once

#include <vulkan/vk_enum_string_helper.h>
#include <Volk/volk.h>

namespace pxl
{
    // Forward declare to avoid cyclic include
    class VulkanDevice;

    class VulkanHelpers
    {
    public:
        static uint32_t GetVulkanAPIVersion();

        static std::vector<VkLayerProperties> GetAvailableInstanceLayers();
        static const char* GetValidationLayer(const std::vector<VkLayerProperties>& availableLayers);

        static std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance);
        static std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice physicalDevice);
        static std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice physicalDevice);

        static std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static std::vector<VkPresentModeKHR> GetSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        static std::optional<uint32_t> GetSuitableGraphicsQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilies, VkPhysicalDevice gpu, VkSurfaceKHR surface);
        static VkSurfaceFormatKHR GetSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
        static VkQueue GetQueueHandle(VkDevice device, const std::optional<uint32_t>& queueIndex);
        static VkPhysicalDevice GetFirstDiscreteGPU(const std::vector<VkPhysicalDevice>& physicalDevices);

        static VkSemaphore CreateSemaphore(VkDevice device);
        static VkFence CreateFence(VkDevice device, bool signaled = false);
    };

    class VulkanDeletionQueue
    {
    public:
        static void Add(std::function<void()> function) { s_Queue.push_back(function); }

        static void Flush();

    private:
        static inline std::vector<std::function<void()>> s_Queue;
    };

#ifdef PXL_DEBUG
    static void CheckVkResult(VkResult result)
    {
        if (result != VK_SUCCESS)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Vulkan Error: {}", string_VkResult(result));
            __debugbreak();
        }
    }
#endif

#ifdef PXL_DEBUG
    #define VK_CHECK(result) CheckVkResult(result)
#else
    #define VK_CHECK(x) x
#endif
}