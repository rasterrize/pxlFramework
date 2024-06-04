#pragma once

#include "../Buffer.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "../BufferLayout.h"
#include "VulkanDevice.h"
#include "VulkanAllocator.h"

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
        static VkFormat GetVkFormatOfBufferDataType(BufferDataType type);
        static VkBufferUsageFlagBits GetVkBufferUsageOfBufferUsage(BufferUsage usage);
    private:
        std::shared_ptr<VulkanDevice> m_Device;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = nullptr;
        VkBufferUsageFlagBits m_Usage;

        // BindBuffer function pointer
        std::function<void(VkCommandBuffer)> m_BindFunc;
    };
}