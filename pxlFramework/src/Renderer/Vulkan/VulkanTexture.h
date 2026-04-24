#pragma once

#include <vma/vk_mem_alloc.h>

#include "Renderer/Texture.h"

namespace pxl
{
    struct VulkanTextureParams
    {
        VkDevice Device = VK_NULL_HANDLE;
        VmaAllocator Allocator = nullptr;
        VkCommandPool OneTimePool = VK_NULL_HANDLE;
        VkQueue GraphicsQueue = VK_NULL_HANDLE;
        float MaxAnisotropyLevel = 1.0f;
    };

    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const TextureSpecs& specs, const VulkanTextureParams& params);

        virtual void Free() override;

        virtual void SetData(const void* data) override;

        virtual void SetAnisotropicLevel(float level) override;

        virtual const TextureSpecs& GetSpecs() const override { return m_Specs; }

        virtual const TextureMetadata& GetMetadata() const override { return m_Metadata; }

        VkImageView GetImageView() const { return m_View; }
        VkSampler GetSampler() const { return m_Sampler; }

    private:
        void InitSampler();

    private:
        TextureSpecs m_Specs = {};
        TextureMetadata m_Metadata = {};

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_View = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkSampler m_Sampler = VK_NULL_HANDLE;

        float m_MaxAnisotropyLevel = 1.0f;
    };
}