#pragma once

#include "../VertexBuffer.h"

#include "../BufferLayout.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanBuffer : public VertexBuffer
    {
    public:
        VulkanBuffer(VkPhysicalDevice gpu, VkDevice device, VkBufferUsageFlagBits usage, uint32_t size, const void* data); // physical device is for getting memory properties, it should be in the VulkanContext or VulkanDevice classes respectively
        ~VulkanBuffer();

        virtual void Bind() override {};
        virtual void Unbind() override {};

        void Bind(VkCommandBuffer commandBuffer);

        virtual void SetData(uint32_t size, const void* data) override;

        void Destroy();

        static VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout);                   // }   Could these be Helper functions?
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(const BufferLayout& layout); // }
        
    private:
        void CreateBuffer(VkBufferUsageFlags usage, uint32_t size);
        void AllocateMemory();
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    private:

        static VkFormat GetVkFormatOfBufferDataType(BufferDataType type);
    private:
        VkPhysicalDevice m_GPU = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_Memory = VK_NULL_HANDLE;

        VkBufferUsageFlagBits m_Usage;
    };
}