#include "VulkanGPUBuffer.h"

#include "VulkanUtils.h"

namespace pxl
{
    VulkanGPUBuffer::VulkanGPUBuffer(const GPUBufferSpecs& specs, VkDevice device, VmaAllocator allocator, VkCommandPool oneTimePool, VkQueue graphicsQueue)
        : m_Specs(specs), m_Device(device), m_Allocator(allocator), m_Usage(VulkanUtils::ToVkBufferUsage(specs.Usage)), m_OneTimeCommandPool(oneTimePool), m_GraphicsQueue(graphicsQueue)
    {
        PXL_ASSERT(specs.Size > 0);

        bool useStagingBuffer = specs.DrawHint == GPUBufferDrawHint::Static;

        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = specs.Size;
        bufferInfo.usage = useStagingBuffer ? m_Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT : m_Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = useStagingBuffer ? VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
                                           : VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

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
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(offset + size <= m_Specs.Size);

        switch (m_Specs.DrawHint)
        {
            case GPUBufferDrawHint::Static:
            {
                VulkanStagingBuffer stagingBuffer(m_Allocator, size, data);

                // Allocate one time command buffer
                auto cmdBuffer = VulkanUtils::AllocateCommandBuffer(m_Device, m_OneTimeCommandPool);

                VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

                VkBufferCopy copyRegion = {};
                copyRegion.srcOffset = 0;
                copyRegion.dstOffset = offset;
                copyRegion.size = m_Specs.Size;
                stagingBuffer.CopyToBuffer(cmdBuffer, m_Buffer, copyRegion);

                VK_CHECK(vkEndCommandBuffer(cmdBuffer));

                VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &cmdBuffer;

                VkFence submitFence = VulkanUtils::CreateFence(m_Device, false);
                vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, submitFence);

                VK_CHECK(vkWaitForFences(m_Device, 1, &submitFence, VK_TRUE, UINT64_MAX));
                vkDestroyFence(m_Device, submitFence, nullptr);

                return;
            }
            case GPUBufferDrawHint::Dynamic:
            {
                VK_CHECK(vmaCopyMemoryToAllocation(m_Allocator, data, m_Allocation, offset, size));
                return;
            }
            default:
            {
                PXL_LOG_ERROR(LogArea::Vulkan, "Failed to set GPUBuffer data, GPUBufferDrawHint is unsupported");
                return;
            }
        }
    }
}