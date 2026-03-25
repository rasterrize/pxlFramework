#pragma once

#include "Renderer/Texture.h"

#include <vma/vk_mem_alloc.h>

namespace pxl
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const TextureSpecs& specs, const std::shared_ptr<Image> image, VkDevice device, VmaAllocator allocator);

        virtual void Free() override;

        virtual void SetData(const void* data) override;

    private:
        VkImage m_Image = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
    };
}