#include "VulkanBuffer.h"

#include "Renderer/BufferLayout.h"
#include "Renderer/Renderer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanHelpers.h"

namespace pxl
{
    VulkanBuffer::VulkanBuffer(GPUBufferUsage usage, GPUBufferDrawHint drawHint, uint32_t size, const void* data)
        : m_Device(static_pointer_cast<VulkanDevice>(Renderer::GetGraphicsContext()->GetDevice())), m_Usage(GetVkBufferUsageOfBufferUsage(usage))
    {
        bool useStagingBuffer = false;
        switch (drawHint)
        {
            case GPUBufferDrawHint::Static:
                useStagingBuffer = true;
                break;

            case GPUBufferDrawHint::Dynamic:
                useStagingBuffer = false;
                break;
        }

        // Staging buffer
        if (useStagingBuffer)
        {
            m_StagingBuffer = CreateStagingBuffer(size);
            m_UploadCommandBuffer = m_Device->AllocateCommandBuffers(QueueType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1).at(0);
        }

        // Dedicated Buffer (actual buffer)
        {
            VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            bufferInfo.size = size;
            bufferInfo.usage = useStagingBuffer ? m_Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT : m_Usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = useStagingBuffer ? VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
                                               : VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            // Create buffer and its associated memory
            VK_CHECK(vmaCreateBuffer(VulkanAllocator::Get(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr));
        }

        VulkanDeletionQueue::Add([&]()
        {
            Destroy();
        });

        // Set the data inside the memory of the buffer
        if (data)
            SetData(size, data);

        // Set appropriate bind function
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) // also could this be a AND (&&) bit operation, would that be faster?
            m_BindFunc = [&](VkCommandBuffer commandBuffer)
            {
                VkBuffer buffers[] = { m_Buffer };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
            };
        else if (m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            m_BindFunc = [&](VkCommandBuffer commandBuffer)
            {
                VkDeviceSize offset = 0;
                vkCmdBindIndexBuffer(commandBuffer, m_Buffer, offset, VK_INDEX_TYPE_UINT32);
            };
        else if (m_Usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        {
        }
        //m_BindFunc = [&](VkCommandBuffer commandBuffer) {};
        else
        {
            PXL_LOG_ERROR(LogArea::Vulkan, "Invalid Vulkan buffer usage specified");
        }
    }

    void VulkanBuffer::Bind()
    {
        PXL_PROFILE_SCOPE;

        auto commandBuffer = std::static_pointer_cast<VulkanGraphicsContext>(Renderer::GetGraphicsContext())->GetSwapchain()->GetCurrentFrame().CommandBuffer;

        m_BindFunc(commandBuffer);
    }

    void VulkanBuffer::Bind(VkCommandBuffer commandBuffer)
    {
        PXL_PROFILE_SCOPE;

        m_BindFunc(commandBuffer);
    }

    void VulkanBuffer::SetData(uint32_t size, const void* data)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT_MSG(data, "Data invalid");
        PXL_ASSERT_MSG(size >= 0, "Size invalid");

        auto context = std::static_pointer_cast<VulkanGraphicsContext>(Renderer::GetGraphicsContext());

        // Use staging buffer if it exists
        if (m_StagingBuffer.Buffer)
        {
            // Fill the vertex buffer with the data
            PXL_PROFILE_SCOPE_NAMED("Mapped memory copy");
            memcpy(m_StagingBuffer.AllocInfo.pMappedData, data, static_cast<size_t>(size));

            // Copy staging buffer contents to dedicated buffer contents
            VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
            beginInfo.flags = 0;

            VK_CHECK(vkBeginCommandBuffer(m_UploadCommandBuffer, &beginInfo));

            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;

            vkCmdCopyBuffer(m_UploadCommandBuffer, m_StagingBuffer.Buffer, m_Buffer, 1, &copyRegion);

            VK_CHECK(vkEndCommandBuffer(m_UploadCommandBuffer));

            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &m_UploadCommandBuffer;

            m_Device->SubmitCommandBuffer(submitInfo, QueueType::Graphics, nullptr);

            {
                PXL_PROFILE_SCOPE_NAMED("Wait for buffer staging upload");
                m_Device->QueueWaitIdle(QueueType::Graphics);
            }
        }
        else
        {
            PXL_PROFILE_SCOPE_NAMED("Mapped memory copy");
            VmaAllocationInfo allocInfo = {};
            vmaGetAllocationInfo(VulkanAllocator::Get(), m_Allocation, &allocInfo);
            memcpy(allocInfo.pMappedData, data, (size_t)size);
        }
    }

    void VulkanBuffer::Destroy()
    {
        if (m_Buffer)
        {
            vmaDestroyBuffer(VulkanAllocator::Get(), m_Buffer, m_Allocation);
            m_Buffer = VK_NULL_HANDLE;
            m_Allocation = VK_NULL_HANDLE;
        }

        if (m_StagingBuffer.Buffer)
            m_StagingBuffer.Destroy();
    }

    VulkanStagingBuffer VulkanBuffer::CreateStagingBuffer(uint32_t size)
    {
        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer stagingBuffer;
        VmaAllocation stagingAllocation;
        VmaAllocationInfo allocationInfo;
        VK_CHECK(vmaCreateBuffer(VulkanAllocator::Get(), &bufferInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &allocationInfo));

        return { stagingBuffer, stagingAllocation, allocationInfo };
    }

    VkVertexInputBindingDescription VulkanBuffer::GetBindingDescription(const BufferLayout& layout)
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = layout.GetStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanBuffer::GetAttributeDescriptions(const BufferLayout& layout)
    {
        auto elements = layout.GetElements();

        std::vector<VkVertexInputAttributeDescription> vertexAttributes(elements.size());

        uint32_t offset = 0;
        for (size_t i = 0; i < elements.size(); i++)
        {
            auto element = elements[i];

            vertexAttributes[i].binding = 0;
            vertexAttributes[i].format = GetVkFormatOfBufferDataType(element.Type);
            vertexAttributes[i].location = static_cast<uint32_t>(i);
            vertexAttributes[i].offset = offset;
            offset += SizeOfBufferDataType(element.Type);
        }

        return vertexAttributes;
    }

    VkFormat VulkanBuffer::GetVkFormatOfBufferDataType(BufferDataType type)
    {
        switch (type)
        {
            case BufferDataType::Float:  return VK_FORMAT_R32_SFLOAT;
            case BufferDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
            case BufferDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
            case BufferDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
            case BufferDataType::Int:    return VK_FORMAT_R32_SINT;
            case BufferDataType::Int2:   return VK_FORMAT_R32G32_SINT;
            case BufferDataType::Int3:   return VK_FORMAT_R32G32B32_SINT;
            case BufferDataType::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
            case BufferDataType::Bool:   return VK_FORMAT_R32_SINT; // I have no idea if this is correct
        }
        return VK_FORMAT_UNDEFINED;
    }

    VkBufferUsageFlagBits VulkanBuffer::GetVkBufferUsageOfBufferUsage(GPUBufferUsage usage)
    {
        switch (usage)
        {
            case GPUBufferUsage::None:
                PXL_LOG_WARN(LogArea::Vulkan, "Buffer usage was none, can't convert to VkBufferUsage");
                break;
            case GPUBufferUsage::Vertex:  return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case GPUBufferUsage::Index:   return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            case GPUBufferUsage::Uniform: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }
}