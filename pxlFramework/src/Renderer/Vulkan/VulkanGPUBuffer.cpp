#include "VulkanGPUBuffer.h"

#include "VulkanUtils.h"

namespace pxl
{
    VulkanGPUBuffer::VulkanGPUBuffer(const GPUBufferSpecs& specs, VkDevice device, VmaAllocator allocator)
        : GPUBuffer(specs), m_Allocator(allocator), m_Usage(VulkanUtils::ToVkBufferUsage(specs.Usage))
    {
        bool useStagingBuffer = false;
        switch (specs.DrawHint)
        {
            case GPUBufferDrawHint::Static:
                useStagingBuffer = true;
                break;

            case GPUBufferDrawHint::Dynamic:
                useStagingBuffer = false;
                break;
        }

        // TODO
        // Staging buffer

        if (useStagingBuffer)
        {
            VulkanStagingBuffer stagingBuffer(allocator, specs.Size);
            // m_UploadCommandBuffer = m_Device->AllocateCommandBuffers(QueueType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1).at(0);
        }

        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = specs.Size;
        bufferInfo.usage = useStagingBuffer ? m_Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT : m_Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = useStagingBuffer ? VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
                                           : VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        // Create buffer and its associated memory
        VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr));

        if (m_Usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        {
            // Get the buffers address as we might need it later
            VkBufferDeviceAddressInfo addressInfo = { VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
            addressInfo.buffer = m_Buffer;

            m_DeviceAddress = vkGetBufferDeviceAddress(device, &addressInfo);
        }

        // Set the data inside the memory of the buffer
        if (specs.Data)
            SetData(specs.Size, specs.Offset, specs.Data);
    }

    void VulkanGPUBuffer::Free()
    {
        if (m_Buffer)
        {
            vmaDestroyBuffer(m_Allocator, m_Buffer, m_Allocation);
            m_Buffer = VK_NULL_HANDLE;
            m_Allocation = VK_NULL_HANDLE;
        }
    }

    void VulkanGPUBuffer::SetData(uint64_t size, uint64_t offset, const void* data)
    {
        // TODO: support staging buffer
        VK_CHECK(vmaCopyMemoryToAllocation(m_Allocator, data, m_Allocation, offset, size));
    }
}