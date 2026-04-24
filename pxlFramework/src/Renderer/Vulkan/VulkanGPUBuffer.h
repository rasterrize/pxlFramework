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
        VulkanStagingBuffer(VmaAllocator allocator, VkDeviceSize size, const void* data)
            : m_Allocator(allocator)
        {
            VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            bufferInfo.size = size;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VK_CHECK(vmaCreateBuffer(m_Allocator, &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr));

            VK_CHECK(vmaCopyMemoryToAllocation(m_Allocator, data, m_Allocation, 0, size));
        }

        ~VulkanStagingBuffer()
        {
            vmaDestroyBuffer(m_Allocator, m_Buffer, m_Allocation);
            m_Buffer = VK_NULL_HANDLE;
            m_Allocation = VK_NULL_HANDLE;
        }

        void CopyToBuffer(VkCommandBuffer cmdBuffer, VkBuffer dstBuffer, const VkBufferCopy& copyRegion)
        {
            vkCmdCopyBuffer(cmdBuffer, m_Buffer, dstBuffer, 1, &copyRegion);
        }

        void CopyToImage(VkCommandBuffer cmdBuffer, VkImage dstImage, VkImageLayout dstImageLayout, const std::vector<VkBufferImageCopy>& copyRegions)
        {
            vkCmdCopyBufferToImage(cmdBuffer, m_Buffer, dstImage, dstImageLayout, static_cast<uint32_t>(copyRegions.size()), copyRegions.data());
        }

    private:
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
    };

    class VulkanGPUBuffer : public GPUBuffer
    {
    public:
        VulkanGPUBuffer(const GPUBufferSpecs& specs, VkDevice device, VmaAllocator allocator, VkCommandPool oneTimePool, VkQueue graphicsQueue);

        virtual void Free() override;

        virtual void SetData(uint64_t size, uint64_t offset, const void* data) override;

        virtual const GPUBufferSpecs& GetSpecs() const override { return m_Specs; }

        VkBuffer GetVkBuffer() const { return m_Buffer; }

        VkBufferUsageFlagBits GetVkBufferUsage() const { return m_Usage; }

        VkDeviceAddress GetDeviceAddress() const { return m_DeviceAddress; }

    private:
        GPUBufferSpecs m_Specs = {};

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = nullptr;
        VkBufferUsageFlagBits m_Usage = {};
        VkDeviceAddress m_DeviceAddress = UINT64_MAX;
        VkDevice m_Device = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
        VkCommandPool m_OneTimeCommandPool = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    };
}