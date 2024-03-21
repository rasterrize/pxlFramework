#include "VulkanDevice.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily)
        : m_PhysicalDevice(physicalDevice), m_GraphicsQueueFamilyIndex(graphicsQueueFamily)
    {
        CreateLogicalDevice(m_PhysicalDevice);
    }

    VulkanDevice::~VulkanDevice()
    {
        Destroy();
    }

    void pxl::VulkanDevice::Destroy()
    {
        if (m_LogicalDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_LogicalDevice, nullptr);
            m_LogicalDevice = VK_NULL_HANDLE;
        }
    }

    void VulkanDevice::CreateLogicalDevice(VkPhysicalDevice gpu)
    {
        if (gpu == VK_NULL_HANDLE)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create logical device, physical device was null pointer");
            return;
        }

        // Specify Device Queue Create Infos
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        uint32_t queueCount = 0;

        if (m_GraphicsQueueFamilyIndex.has_value())
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

        auto availableExtensions = VulkanHelpers::GetDeviceExtensions(m_PhysicalDevice);

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
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to enable all selected device extensions");
            return;
        }

        // Specify Device Create Info
        VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());;
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult result = vkCreateDevice(gpu, &deviceInfo, nullptr, &m_LogicalDevice);
        VulkanHelpers::CheckVkResult(result);

        if (!m_LogicalDevice)
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Failed to create logical device");
        }
        
        PXL_LOG_INFO(LogArea::Vulkan, "Logical Device successfully created:");
        PXL_LOG_INFO(LogArea::Vulkan, "   {} enabled extensions: ", enabledExtensionCount);
        
        for (uint32_t i = 0; i < enabledExtensionCount; i++)
        {
            PXL_LOG_INFO(LogArea::Vulkan, "   - {}", deviceExtensions[i]);
        }
        
        // TODO: expand this
    }

    uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        // Find correct memory type
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProps);

        // Loop through each type of memory and check if the specified type matches as well as the properties
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        return -1;
    }
}