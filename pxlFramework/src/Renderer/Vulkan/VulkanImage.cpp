#include "VulkanImage.h"

#include "VulkanBuffer.h"
#include "VulkanHelpers.h"
#include "Renderer/Renderer.h"

namespace pxl
{
    VulkanImage::VulkanImage(uint32_t width, uint32_t height, VkFormat format)
        : m_Device(static_cast<VkDevice>(Renderer::GetGraphicsContext()->GetDevice()->GetLogical())), m_Width(width), m_Height(height), m_Format(format)
    {
        // Create a vulkan image AND image view
        CreateImage(m_Width, m_Height, m_Format);
        //CreateImageView(m_Format, m_Image);
    }

    VulkanImage::VulkanImage(const std::shared_ptr<VulkanDevice>& device, Size2D size, VkFormat format)
        : VulkanImage(device, size.Width, size.Height, format)
    {
    }

    VulkanImage::VulkanImage(const std::shared_ptr<VulkanDevice>& device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage)
        : m_Device(device->GetVkLogical()), m_Width(width), m_Height(height), m_Format(format), m_IsSwapchainImage(true)
    {
        // Create just a vulkan image view for the supplied image
        CreateImageView(m_Format, swapchainImage);
    }

    void VulkanImage::Destroy()
    {
        if (m_Image)
        {
            vmaDestroyImage(VulkanAllocator::Get(), m_Image, m_Allocation);
            m_Image = VK_NULL_HANDLE;
        }

        if (m_ImageView)
        {
            vkDestroyImageView(m_Device, m_ImageView, nullptr);
            m_ImageView = VK_NULL_HANDLE;
        }
    }

    void VulkanImage::SetData(const Image& image)
    {
        // Staging buffer
        uint32_t imageSizeInBytes = image.Metadata.Size.Width * image.Metadata.Size.Height * 4; // TODO: get the number of bytes from the format (RGBA/RGB)

        VulkanStagingBuffer stagingBuffer = VulkanBuffer::CreateStagingBuffer(imageSizeInBytes);

        memcpy(stagingBuffer.AllocInfo.pMappedData, image.Buffer, imageSizeInBytes);

        auto uploadCommandBuffer = m_Device->AllocateCommandBuffers(QueueType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1).at(0);

        // Copy staging buffer contents to dedicated buffer contents
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = 0;

        VK_CHECK(vkBeginCommandBuffer(uploadCommandBuffer, &beginInfo));

        // VkBufferImageCopy copyRegion = {};
        // copyRegion.

        // vkCmdCopyBufferToImage(m_UploadCommandBuffer, stagingBuffer.Buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        VK_CHECK(vkEndCommandBuffer(uploadCommandBuffer));

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &uploadCommandBuffer;

        m_Device->SubmitCommandBuffer(submitInfo, QueueType::Graphics, nullptr);

        {
            PXL_PROFILE_SCOPE_NAMED("Wait for buffer staging upload");
            m_Device->QueueWaitIdle(QueueType::Graphics);
        }

        VulkanDeletionQueue::Add([&]()
        {
            Destroy();
        });
    }

    void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format)
    {
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // TODO: These are for textures ONLY (so I can get it working)
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

        VK_CHECK(vmaCreateImage(VulkanAllocator::Get(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr));
    }

    void VulkanImage::CreateImageView(VkFormat format, VkImage image)
    {
        // Create image view
        VkImageViewCreateInfo imageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imageViewInfo.image = image;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = format;
        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // subresourceRange determines how the image should be accessed
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_ImageView));
    }

    VkFormat VulkanImage::ToVkFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined: return VK_FORMAT_UNDEFINED;
            case ImageFormat::RGB8:      return VK_FORMAT_R8G8B8_SRGB;
            case ImageFormat::RGBA8:     return VK_FORMAT_R8G8B8A8_SRGB;
        }

        return VK_FORMAT_UNDEFINED;
    }
}