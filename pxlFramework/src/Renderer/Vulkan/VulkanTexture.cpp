#include "VulkanTexture.h"

#include <volk/volk.h>

#include "VulkanUtils.h"

namespace pxl
{
    VulkanTexture::VulkanTexture(const TextureSpecs& specs, const std::shared_ptr<Image> image, VkDevice device, VmaAllocator allocator)
        : m_Allocator(allocator)
    {
        VkImageType imageType;
        switch (specs.Type)
        {
            case TextureType::Tex1D: imageType = VK_IMAGE_TYPE_1D; break;
            case TextureType::Tex2D: imageType = VK_IMAGE_TYPE_2D; break;
            case TextureType::Tex3D: imageType = VK_IMAGE_TYPE_3D; break;
        }

        // Create the VkImage object using VMA
        auto metadata = image->Metadata;
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = imageType;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent = { metadata.Size.Width, metadata.Size.Height, 1 };
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // Multi-sampl;ng
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;


        VmaAllocation allocation;
        VK_CHECK(vmaCreateImage(m_Allocator, &imageInfo, &allocInfo, &m_Image, &allocation, nullptr));
        // Create an image view for the image
        // Copy image data from buffer to image object
        // Setup a sampler
    }

    void VulkanTexture::Free()
    {
    }

    void VulkanTexture::SetData(const void* data)
    {
    }
}