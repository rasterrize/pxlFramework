#include "VulkanBuffer.h"

#include "../BufferLayout.h"
#include "../Renderer.h"
#include "VulkanHelpers.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"

namespace pxl
{
    VulkanBuffer::VulkanBuffer(const std::shared_ptr<VulkanDevice>& device, BufferUsage usage, uint32_t size, const void* data)
        : m_Device(device), m_Usage(GetVkBufferUsageOfBufferUsage(usage))
    {
        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = m_Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; // for vmaMapMemory

        // Create buffer and it's associated memory
        VK_CHECK(vmaCreateBuffer(VulkanAllocator::Get(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr));

        VulkanDeletionQueue::Add([&]() {
            Destroy();
        });

        // Set the data inside the memory of the buffer
        SetData(size, data);

        // Set appropriate bind function
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) // also could this be a AND (&&) bit operation, would that be faster?
            m_BindFunc = [&](VkCommandBuffer commandBuffer) { 
                VkBuffer buffers[] = { m_Buffer };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
            };
        else if (m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            m_BindFunc = [&](VkCommandBuffer commandBuffer) { 
                VkDeviceSize offset = 0;
                vkCmdBindIndexBuffer(commandBuffer, m_Buffer, offset, VK_INDEX_TYPE_UINT32);
            };
        else    
            PXL_LOG_ERROR(LogArea::Vulkan, "Invalid Vulkan buffer usage specified");
    }

    VulkanBuffer::~VulkanBuffer()
    {
        Destroy();
    }

    void VulkanBuffer::Bind()
    {
        auto commandBuffer = std::static_pointer_cast<VulkanGraphicsContext>(Renderer::GetGraphicsContext())->GetSwapchain()->GetCurrentFrame().CommandBuffer;

        m_BindFunc(commandBuffer);
    }

    void VulkanBuffer::Bind(VkCommandBuffer commandBuffer)
    {
        m_BindFunc(commandBuffer);
    }

    void VulkanBuffer::SetData(uint32_t size, const void* data)
    {
        if (!data)
            return;
        
        auto allocator = VulkanAllocator::Get();

        // Fill the vertex buffer with the data
        void* mappedMemory;
        vmaMapMemory(allocator, m_Allocation, &mappedMemory);
        memcpy(mappedMemory, data, (size_t)size);
        vmaUnmapMemory(allocator, m_Allocation);
    }

    void VulkanBuffer::Destroy()
    {
        if (m_Buffer)
        {
            vmaDestroyBuffer(VulkanAllocator::Get(), m_Buffer, m_Allocation);
            m_Buffer = VK_NULL_HANDLE;
            m_Allocation = VK_NULL_HANDLE;
        }
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
        for (int i = 0; i < elements.size(); i++)
        {
            auto element = elements[i];
            
            vertexAttributes[i].binding = 0;
            vertexAttributes[i].format = GetVkFormatOfBufferDataType(element.Type);
            vertexAttributes[i].location = i;
            vertexAttributes[i].offset = offset;
            offset += GetSizeOfType(element.Type);
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

    VkBufferUsageFlagBits VulkanBuffer::GetVkBufferUsageOfBufferUsage(BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::None:
                PXL_LOG_WARN(LogArea::Vulkan, "Buffer usage was none, can't convert to VkBufferUsage");
                break;
            case BufferUsage::Vertex:
                return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case BufferUsage::Index:
                return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        
        return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }
}