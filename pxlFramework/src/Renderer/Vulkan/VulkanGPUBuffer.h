#pragma once

#include <vma/vk_mem_alloc.h>
#include <volk/volk.h>

#include "Renderer/GPUBuffer.h"
#include "VulkanUtils.h"

namespace pxl
{
    class VulkanStagingBuffer
    {
    public:
        VulkanStagingBuffer(VmaAllocator allocator, VkDeviceSize size)
            : m_Allocator(allocator)
        {
            VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            bufferInfo.size = size;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VK_CHECK(vmaCreateBuffer(m_Allocator, &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr));
        }

        ~VulkanStagingBuffer()
        {
            vmaDestroyBuffer(m_Allocator, m_Buffer, m_Allocation);
            m_Buffer = VK_NULL_HANDLE;
            m_Allocation = VK_NULL_HANDLE;
        }

    private:
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
    };

    class VulkanGPUBuffer : public GPUBuffer
    {
    public:
        VulkanGPUBuffer(const GPUBufferSpecs& specs, VkDevice device, VmaAllocator allocator);

        virtual void Free() override;

        virtual void SetData(uint64_t size, uint64_t offset, const void* data) override;

        VkBuffer GetVkBuffer() const { return m_Buffer; }

        VkBufferUsageFlagBits GetVkBufferUsage() const { return m_Usage; }

        VkDeviceAddress GetDeviceAddress() const { return m_DeviceAddress; }

    private:
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = nullptr;
        VkBufferUsageFlagBits m_Usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
        VkDeviceAddress m_DeviceAddress = UINT64_MAX;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
    };
}