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

        VkDevice GetVkDevice() const { return m_LogicalDevice; }
        VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }

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
