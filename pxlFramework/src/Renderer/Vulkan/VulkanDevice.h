#pragma once

#include "../GraphicsDevice.h"

#include <vulkan/vulkan.h>

#include "VulkanHelpers.h"

namespace pxl
{
    class VulkanDevice : public GraphicsDevice
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily);
        virtual ~VulkanDevice() override;

        virtual void* GetDevice() override { return m_LogicalDevice; } // TODO: rename to something better (so I don't get code like m_Context->GetDevice()->GetDevice())
        virtual void* GetPhysicalDevice() override { return m_PhysicalDevice; }

        virtual void WaitIdle() override { VK_CHECK(vkDeviceWaitIdle(m_LogicalDevice)); }

        virtual const DeviceLimits& GetDeviceLimits() override { return DeviceLimits(); }; // TODO

        void Destroy();

        uint32_t GetGraphicsQueueIndex() const { return m_GraphicsQueueFamilyIndex.value(); }
        int32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void LogDeviceLimits(); // could be CheckDeviceLimits later so I can ensure correct device compatibility

    private:
        void CreateLogicalDevice(VkPhysicalDevice gpu);
    private:
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;
    };
}
