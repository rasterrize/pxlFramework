#include "VulkanBuffer.h"

#include "../BufferLayout.h"
#include "VulkanHelpers.h"

namespace pxl
{
    VulkanBuffer::VulkanBuffer(VkPhysicalDevice gpu, VkDevice device, VkBufferUsageFlagBits usage, uint32_t size, const void* data)
        : m_Device(device), m_GPU(gpu), m_Usage(usage)
    {
        // Create the vulkan buffer
        CreateBuffer(usage, size);

        // Allocate memory and bind it to the buffer
        AllocateMemory();
        vkBindBufferMemory(m_Device, m_Buffer, m_Memory, 0);

        // Set the data inside the memory of the buffer
        SetData(size, data);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        Destroy();
    }

    void VulkanBuffer::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { m_Buffer };
        VkDeviceSize offsets[] = { 0 };
        
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) // definitely shouldnt be doing this
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        else if (m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
            vkCmdBindIndexBuffer(commandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanBuffer::SetData(uint32_t size, const void* data)
    {
        // Fill the vertex buffer with the data
        void* data2;
        vkMapMemory(m_Device, m_Memory, 0, size, 0, &data2);
        memcpy(data2, data, (size_t)size);
        vkUnmapMemory(m_Device, m_Memory);
    }

    void VulkanBuffer::Destroy()
    {
        vkDeviceWaitIdle(m_Device);
        
        if (m_Buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(m_Device, m_Buffer, nullptr);

        if (m_Memory != VK_NULL_HANDLE)
            vkFreeMemory(m_Device, m_Memory, nullptr);

    }

    void VulkanBuffer::CreateBuffer(VkBufferUsageFlags usage, uint32_t size)
    {
        // Create buffer
        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto result = vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer);
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
            offset += element.Count * GetSizeOfType(element.Type);
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

    void VulkanBuffer::AllocateMemory()
    {
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memReqs);

        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        auto result = vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_Memory);
        VulkanHelpers::CheckVkResult(result);
    }

    uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        // Find correct memory type
        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(m_GPU, &memProps);

        // Loop through each type of memory and check if the specified type matches as well as the properties
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        return -1;
    }
}