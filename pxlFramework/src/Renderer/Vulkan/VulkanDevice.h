#pragma once

#include "../Device.h"

#include <vulkan/vulkan.h>

#include "VulkanHelpers.h"

namespace pxl
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily);
        virtual ~VulkanDevice() override;

        virtual void* GetLogicalDevice() override { return m_LogicalDevice; };

        virtual void Destroy() override;

        VkDevice GetVkDevice() const { return m_LogicalDevice; } // TODO: either GetLogicalDevice is used everywhere (more casting) or this is used idk
        VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }

        uint32_t GetGraphicsQueueIndex() const { return m_GraphicsQueueFamilyIndex.value(); }
        int32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void LogDeviceLimits(); // could be CheckDeviceLimits later so I can ensure correct device compatibility

        void WaitIdle() { VK_CHECK(vkDeviceWaitIdle(m_LogicalDevice)); }

        static void WaitIdle(VkDevice device) { VK_CHECK(vkDeviceWaitIdle(device)); }
    private:
        void CreateLogicalDevice(VkPhysicalDevice gpu);
    private:
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;
    };
}
