#include "VulkanDevice.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        : m_PhysicalDevice(physicalDevice)
    {
        // Get required queue families from the device
        auto queueFamilies = VulkanHelpers::GetQueueFamilies(physicalDevice);

        m_GraphicsQueueFamily = VulkanHelpers::GetSuitableGraphicsQueueFamily(queueFamilies, physicalDevice, surface);

        // Create the logical device
        CreateLogicalDevice(m_PhysicalDevice);

        VulkanDeletionQueue::Add([&]()
        {
            vkDestroyDevice(m_LogicalDevice, nullptr);
            m_LogicalDevice = VK_NULL_HANDLE;
        });

        // Get graphics/present queue from device
        m_GraphicsQueue = VulkanHelpers::GetQueueHandle(m_LogicalDevice, m_GraphicsQueueFamily);

        // Create graphics command pool
        VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamily.value();

        VK_CHECK(vkCreateCommandPool(m_LogicalDevice, &commandPoolInfo, nullptr, &m_GraphicsCommandPool));

        VulkanDeletionQueue::Add([&]()
        {
            vkDestroyCommandPool(m_LogicalDevice, m_GraphicsCommandPool, nullptr);
            m_GraphicsCommandPool = VK_NULL_HANDLE;
        });
    }

    std::vector<VkCommandBuffer> VulkanDevice::AllocateCommandBuffers(QueueType queueType, VkCommandBufferLevel level, uint32_t count)
    {
        std::vector<VkCommandBuffer> commandBuffers(count);

        VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocInfo.commandPool = GetCommandPoolFromQueueType(queueType);
        commandBufferAllocInfo.level = level;
        commandBufferAllocInfo.commandBufferCount = count;

        VK_CHECK(vkAllocateCommandBuffers(m_LogicalDevice, &commandBufferAllocInfo, commandBuffers.data()));

        return commandBuffers;
    }

    void VulkanDevice::SubmitCommandBuffer(const VkSubmitInfo& submitInfo, QueueType queueType, VkFence signalFence)
    {
        PXL_PROFILE_SCOPE;

        auto queue = GetQueueFromQueueType(queueType);

        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, signalFence));
    }

    void VulkanDevice::SubmitCommandBuffers(const std::vector<VkSubmitInfo>& submitInfos, QueueType queueType, VkFence signalFence)
    {
        PXL_PROFILE_SCOPE;

        auto queue = GetQueueFromQueueType(queueType);

        VK_CHECK(vkQueueSubmit(queue, 1, submitInfos.data(), signalFence));
    }

    void VulkanDevice::SubmitPresent(const VkPresentInfoKHR& presentInfo)
    {
        PXL_PROFILE_SCOPE;

        VK_CHECK(vkQueuePresentKHR(m_GraphicsQueue, &presentInfo));
    }

    void VulkanDevice::CreateLogicalDevice(VkPhysicalDevice gpu)
    {
        PXL_ASSERT_MSG(gpu != VK_NULL_HANDLE, "Failed to create logical device, physical device was null")

        // Specify Device Queue Create Infos
        std::vector<VkDeviceQueueCreateInfo> queueInfos;

        if (m_GraphicsQueueFamily.has_value())
        {
            float queuePriority = 1.0f;
            VkDeviceQueueCreateInfo graphicsQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
            graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily.value();
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
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VK_CHECK(vkCreateDevice(gpu, &deviceInfo, nullptr, &m_LogicalDevice));

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

    VkQueue VulkanDevice::GetQueueFromQueueType(QueueType type)
    {
        switch (type)
        {
            case QueueType::Graphics: return m_GraphicsQueue;
            case QueueType::Compute:  PXL_LOG_ERROR(LogArea::Vulkan, "Compute queues are unsupported");
        }

        return VK_NULL_HANDLE;
    }

    VkCommandPool VulkanDevice::GetCommandPoolFromQueueType(QueueType type)
    {
        switch (type)
        {
            case QueueType::Graphics: return m_GraphicsCommandPool;
            case QueueType::Compute:  PXL_LOG_ERROR(LogArea::Vulkan, "Compute command pools are unsupported");
        }

        return VK_NULL_HANDLE;
    }

    [[deprecated]] int32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

    void VulkanDevice::LogDeviceLimits()
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

        PXL_LOG_INFO(LogArea::Vulkan, "Selected discrete GPU: {}", properties.deviceName); // TODO: move this because it doesn't necessarily mean it has been selected
        PXL_LOG_INFO(LogArea::Vulkan, "- Vendor: {}", properties.vendorID);
        PXL_LOG_INFO(LogArea::Vulkan, "- Driver Version: {}", properties.driverVersion);
        PXL_LOG_INFO(LogArea::Vulkan, "- Supported Vulkan API Version: {}", properties.apiVersion);
        PXL_LOG_INFO(LogArea::Vulkan, "- Limits");
        PXL_LOG_INFO(LogArea::Vulkan, " - Max Push Constant Size: {} bytes", properties.limits.maxPushConstantsSize);
        PXL_LOG_INFO(LogArea::Vulkan, " - Max Draw Indexed Index Value: {}", properties.limits.maxDrawIndexedIndexValue);
        PXL_LOG_INFO(LogArea::Vulkan, " - Max Memory Allocation Count: {}", properties.limits.maxMemoryAllocationCount);
        PXL_LOG_INFO(LogArea::Vulkan, " - Max Memory Heaps Count: {}", memoryProperties.memoryHeapCount);
        PXL_LOG_INFO(LogArea::Vulkan, " - Memory Heap 1 Size: {}", memoryProperties.memoryHeaps->size);
    }
}