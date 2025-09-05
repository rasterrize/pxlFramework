#pragma once

#include <vma/vk_mem_alloc.h>
#include <Volk/volk.h>

#include "Renderer/BufferLayout.h"
#include "Renderer/GPUBuffer.h"
#include "VulkanAllocator.h"
#include "VulkanDevice.h"

namespace pxl
{
    struct VulkanStagingBuffer
    {
        VkBuffer Buffer = VK_NULL_HANDLE;
        VmaAllocation Allocation = VK_NULL_HANDLE;
        VmaAllocationInfo AllocInfo = {};

        void Destroy()
        {
            vmaDestroyBuffer(VulkanAllocator::Get(), Buffer, Allocation);
            Buffer = VK_NULL_HANDLE;
            Allocation = VK_NULL_HANDLE;
        }
    };

    class VulkanBuffer : public GPUBuffer
    {
    public:
        VulkanBuffer(GPUBufferUsage usage, GPUBufferDrawHint drawHint, uint32_t size, const void* data);

        virtual void Bind() override;
        virtual void Unbind() override {}

        void Bind(VkCommandBuffer commandBuffer);

        virtual void SetData(uint32_t size, const void* data) override;

        void Destroy();

        static VulkanStagingBuffer CreateStagingBuffer(uint32_t size);

        static VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout);                   // }   Could these be Helper functions?
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(const BufferLayout& layout); // }

    private:
        static VkFormat GetVkFormatOfBufferDataType(BufferDataType type);
        static VkBufferUsageFlagBits GetVkBufferUsageOfBufferUsage(GPUBufferUsage usage);

    private:
        std::shared_ptr<VulkanDevice> m_Device = nullptr;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = nullptr;
        VkBufferUsageFlagBits m_Usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
        std::function<void(VkCommandBuffer)> m_BindFunc = nullptr;

        // Staging data
        VulkanStagingBuffer m_StagingBuffer = {};
        VkCommandBuffer m_UploadCommandBuffer = VK_NULL_HANDLE;
    };
}