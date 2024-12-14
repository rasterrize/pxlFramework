#include "VulkanImage.h"

#include "Renderer/Renderer.h"
#include "VulkanBuffer.h"
#include "VulkanHelpers.h"

namespace pxl
{

    VulkanImage::VulkanImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags)
        : m_Device(static_pointer_cast<VulkanDevice>(Renderer::GetGraphicsContext()->GetDevice())), m_Width(width), m_Height(height), m_Format(format)
    {
        // Create a vulkan image AND image view
        CreateImage(m_Width, m_Height, m_Format, usageFlags);
        CreateImageView(m_Format, m_Image);
    }

    VulkanImage::VulkanImage(const std::shared_ptr<GraphicsDevice>& device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage)
        : m_Device(static_pointer_cast<VulkanDevice>(device)), m_Image(swapchainImage), m_Width(width), m_Height(height), m_Format(format), m_IsSwapchainImage(true)
    {
        // Create just a vulkan image view for the supplied image
        CreateImageView(m_Format, swapchainImage);
    }

    void VulkanImage::Destroy()
    {
        if (!m_IsSwapchainImage)
        {
            if (m_Image)
            {
                vmaDestroyImage(VulkanAllocator::Get(), m_Image, m_Allocation);
                m_Image = VK_NULL_HANDLE;
            }
        }

        if (m_ImageView)
        {
            vkDestroyImageView(static_cast<VkDevice>(m_Device->GetLogical()), m_ImageView, nullptr);
            m_ImageView = VK_NULL_HANDLE;
        }
    }

    void VulkanImage::SetData(const Image& image)
    {
        // Staging buffer
        uint32_t imageSizeInBytes = image.Metadata.Size.Width * image.Metadata.Size.Height * Utils::ImageFormatToBytes(image.Metadata.Format);

        VulkanStagingBuffer stagingBuffer = VulkanBuffer::CreateStagingBuffer(imageSizeInBytes);

        memcpy(stagingBuffer.AllocInfo.pMappedData, image.Buffer.data(), imageSizeInBytes);

        auto uploadCommandBuffer = m_Device->AllocateCommandBuffers(QueueType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1).at(0);

        // Copy staging buffer contents to dedicated buffer contents
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = 0;

        VK_CHECK(vkBeginCommandBuffer(uploadCommandBuffer, &beginInfo));

        // Transition the image layout into the optimal transfer layout
        TransitionImageLayout(uploadCommandBuffer, m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Copy the buffer contents into the image object
        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        
        copyRegion.imageOffset = { 0, 0, 0 };
        copyRegion.imageExtent = { m_Width, m_Height, 1 };

        vkCmdCopyBufferToImage(uploadCommandBuffer, stagingBuffer.Buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        // Transition the image layout into a shader readable format // TODO: this assumes the image is used for a texture
        TransitionImageLayout(uploadCommandBuffer, m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VK_CHECK(vkEndCommandBuffer(uploadCommandBuffer));

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &uploadCommandBuffer;

        m_Device->SubmitCommandBuffer(submitInfo, QueueType::Graphics, nullptr);

        {
            PXL_PROFILE_SCOPE_NAMED("Wait for buffer staging upload");
            m_Device->QueueWaitIdle(QueueType::Graphics);
        }

        stagingBuffer.Destroy();

        VulkanDeletionQueue::Add([&]()
        {
            Destroy();
        });
    }

    void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags)
    {
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VK_IMAGE_TYPE_2D; // TODO: currently assumes all images are 2D
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usageFlags;
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

        VK_CHECK(vkCreateImageView(m_Device->GetVkLogical(), &imageViewInfo, nullptr, &m_ImageView));
    }

    void VulkanImage::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.image = image;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        // TODO: understand and adjust to be more flexible
        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, // Which operations before the barrier
            destinationStage, // Which operations to wait on the barrier
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
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