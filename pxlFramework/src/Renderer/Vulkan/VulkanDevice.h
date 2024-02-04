// inherits from Device.h
// Holds Physical and Logical devices
// Can be used to query information such as the supported capabilities from a physical device and surface
// May or may not be used to allocate memory on the device

#pragma once
#include "../Device.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily);
        ~VulkanDevice();

        virtual void* GetLogicalDevice() override { return m_LogicalDevice; }
        virtual void* GetPhysicalAdapter() override { return m_PhysicalDevice; }

        virtual void Destroy() override;
    
        uint32_t GetGraphicsQueueIndex() const { return m_GraphicsQueueFamilyIndex.value(); }
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    private:
        void CreateLogicalDevice(VkPhysicalDevice gpu);
    private:
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        std::optional<uint32_t> m_GraphicsQueueFamilyIndex;
    };
}
