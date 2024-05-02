#pragma once

#include "../Buffer.h"

#include <vulkan/vulkan.h>

#include "../BufferLayout.h"
#include "VulkanDevice.h"

namespace pxl
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(const std::shared_ptr<VulkanDevice>& device, BufferUsage usage, uint32_t size, const void* data); // physical device is for getting memory properties, it should be in the VulkanContext or VulkanDevice classes respectively
        virtual ~VulkanBuffer() override;

        virtual void Bind() override;
        virtual void Unbind() override {}

        void Bind(VkCommandBuffer commandBuffer);

        virtual void SetData(uint32_t size, const void* data) override;

        void Destroy();

        static VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout);                   // }   Could these be Helper functions?
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(const BufferLayout& layout); // }
        
    private:
        void CreateBuffer(VkBufferUsageFlags usage, uint32_t size);
        void AllocateMemory();

        void BindVertexImpl(VkCommandBuffer commandBuffer);
        void BindIndexImpl(VkCommandBuffer commandBuffer);
    private:
        static VkFormat GetVkFormatOfBufferDataType(BufferDataType type);
        static VkBufferUsageFlagBits GetVkBufferUsageOfBufferUsage(BufferUsage usage);
    private:
        std::shared_ptr<VulkanDevice> m_Device;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_Memory = VK_NULL_HANDLE;
        VkBufferUsageFlagBits m_Usage;

        std::function<void(VkCommandBuffer)> m_BindFunc;
        // BindBuffer function pointer
        //void(*m_VkBindFunc)();
    };
}