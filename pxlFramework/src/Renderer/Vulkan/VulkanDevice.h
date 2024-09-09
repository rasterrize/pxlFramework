#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/GraphicsDevice.h"
#include "VulkanHelpers.h"

namespace pxl
{
    class VulkanDevice : public GraphicsDevice
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily);
        virtual ~VulkanDevice() override;

        virtual void* GetLogical() const override { return m_LogicalDevice; }
        virtual void* GetPhysical() const override { return m_PhysicalDevice; }

        virtual void WaitIdle() const override { VK_CHECK(vkDeviceWaitIdle(m_LogicalDevice)); }

        virtual const DeviceLimits& GetDeviceLimits() const override { return m_DeviceLimits; }

        void Destroy();

        uint32_t GetGraphicsQueueIndex() const { return m_GraphicsQueueFamilyIndex.value(); }
        int32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void LogDeviceLimits(); // could be CheckDeviceLimits later so I can ensure correct device compatibility
    private:
        void CreateLogicalDevice(VkPhysicalDevice gpu);
    private:
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        DeviceLimits m_DeviceLimits;

        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;
    };
}
