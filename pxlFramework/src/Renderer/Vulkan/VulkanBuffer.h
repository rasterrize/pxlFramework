#pragma once

#include "../GPUBuffer.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "../BufferLayout.h"
#include "VulkanDevice.h"
#include "VulkanAllocator.h"

namespace pxl
{
    class VulkanBuffer : public GPUBuffer
    {
    public:
        VulkanBuffer(const std::shared_ptr<VulkanDevice>& device, GPUBufferUsage usage, uint32_t size, const void* data);
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
        static VkBufferUsageFlagBits GetVkBufferUsageOfBufferUsage(GPUBufferUsage usage);
    private:
        std::shared_ptr<VulkanDevice> m_Device;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = nullptr;
        VkBufferUsageFlagBits m_Usage;

        std::function<void(VkCommandBuffer)> m_BindFunc;
    };
}