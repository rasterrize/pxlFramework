#pragma once
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "Renderer/Texture.h"
#include "VulkanAllocator.h"
#include "VulkanImage.h"

namespace pxl
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const Image& image, const TextureSpecs& specs);

        virtual void Bind([[maybe_unused]] uint32_t unit) override {}; // unsure if this is used in Vulkan
        virtual void Unbind() override {};

        virtual void SetData(const void* data) override;

        virtual const ImageMetadata& GetMetadata() const override { return m_Metadata; }

        void Destroy();

    private:
        VkFilter ToVkFilter(SampleFilter filter);
        VkSamplerAddressMode ToVkAddressMode(TextureWrapMode mode);

    private:
        std::shared_ptr<GraphicsDevice> m_Device = nullptr;
        std::unique_ptr<VulkanImage> m_Image = nullptr;
        VkSampler m_Sampler = VK_NULL_HANDLE;

        // Staging data
        VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
        VmaAllocation m_StagingAllocation = nullptr;
        VkFence m_UploadFence = VK_NULL_HANDLE;
        VkCommandBuffer m_UploadCommandBuffer = VK_NULL_HANDLE;

        ImageMetadata m_Metadata = {};
    };
}