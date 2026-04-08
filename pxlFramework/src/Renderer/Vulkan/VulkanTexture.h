#pragma once

#include <vma/vk_mem_alloc.h>

#include "Renderer/Texture.h"

namespace pxl
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const TextureSpecs& specs, VkDevice device, VmaAllocator allocator, VkCommandPool oneTimePool, VkQueue graphicsQueue);

        virtual void Free() override;

        virtual void SetData(const void* data) override;

        virtual void SetAnisotropicLevel(float level) override;

        virtual const TextureSpecs& GetSpecs() const override { return m_Specs; }

        virtual const TextureMetadata& GetMetadata() const override { return m_Metadata; }

        VkImageView GetImageView() const { return m_View; }
        VkSampler GetSampler() const { return m_Sampler; }

    private:
        TextureSpecs m_Specs = {};
        TextureMetadata m_Metadata = {};

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_View = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkSampler m_Sampler = VK_NULL_HANDLE;
    };
}