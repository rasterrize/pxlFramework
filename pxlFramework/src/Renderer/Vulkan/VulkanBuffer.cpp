#include "VulkanBuffer.h"

#include "../BufferLayout.h"
#include "VulkanHelpers.h"

#include "../Renderer.h"

#include "VulkanContext.h"

namespace pxl
{
    VulkanBuffer::VulkanBuffer(const std::shared_ptr<VulkanDevice> device, BufferUsage usage, uint32_t size, const void* data)
        : m_Device(device)
    {
        // Generic buffer usage to vulkan buffer usage
        m_Usage = GetVkBufferUsageOfBufferUsage(usage);

        // Create the vulkan buffer
        CreateBuffer(m_Usage, size);

        // Allocate memory and bind it to the buffer
        AllocateMemory();
        vkBindBufferMemory(m_Device->GetVkDevice(), m_Buffer, m_Memory, 0);

        // Set the data inside the memory of the buffer
        SetData(size, data);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        Destroy();
    }

    void VulkanBuffer::Bind()
    {
        VkBuffer buffers[] = { m_Buffer };
        VkDeviceSize offsets[] = { 0 };

        auto commandBuffer = std::dynamic_pointer_cast<VulkanContext>(Renderer::GetGraphicsContext())->GetCurrentFrame().CommandBuffer;
        
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) // going to do this for now as it simplifies things a little // also could this be a AND (&&) bit operation, would that be faster?
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        else if (m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            vkCmdBindIndexBuffer(commandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanBuffer::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { m_Buffer };
        VkDeviceSize offsets[] = { 0 };
        
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) // going to do this for now as it simplifies things a little // also could this be a AND (&&) bit operation, would that be faster?
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        else if (m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            vkCmdBindIndexBuffer(commandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanBuffer::SetData(uint32_t size, const void* data)
    {
        auto device = m_Device->GetVkDevice();

        // Fill the vertex buffer with the data
        void* data2;
        vkMapMemory(device, m_Memory, 0, size, 0, &data2);
        memcpy(data2, data, (size_t)size);
        vkUnmapMemory(device, m_Memory);
    }

    void VulkanBuffer::Destroy()
    {
        auto device = m_Device->GetVkDevice();
        vkDeviceWaitIdle(device);
        
        if (m_Buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(device, m_Buffer, nullptr);

        if (m_Memory != VK_NULL_HANDLE)
            vkFreeMemory(device, m_Memory, nullptr);

    }

    void VulkanBuffer::CreateBuffer(VkBufferUsageFlags usage, uint32_t size)
    {
        // Create buffer
        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto result = vkCreateBuffer(m_Device->GetVkDevice(), &bufferInfo, nullptr, &m_Buffer);
        VulkanHelpers::CheckVkResult(result);
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

    void VulkanBuffer::AllocateMemory()
    {
        auto logicalDevice = m_Device->GetVkDevice();
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(logicalDevice, m_Buffer, &memReqs);

        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = m_Device->FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        auto result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &m_Memory);
        VulkanHelpers::CheckVkResult(result);
    }
}