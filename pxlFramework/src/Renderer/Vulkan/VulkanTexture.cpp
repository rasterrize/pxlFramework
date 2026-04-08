#include "VulkanTexture.h"

#include <volk/volk.h>

#include "VulkanGPUBuffer.h"
#include "VulkanUtils.h"

namespace pxl
{
    VulkanTexture::VulkanTexture(const TextureSpecs& specs, VkDevice device, VmaAllocator allocator, VkCommandPool oneTimePool, VkQueue graphicsQueue)
        : m_Specs(specs), m_Allocator(allocator), m_Device(device)
    {
        m_Metadata = { m_Specs.Image->Metadata.Size };

        // All our textures are in this format for now
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

        // Create the VkImage object using VMA
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VulkanUtils::ToVkImageType(m_Specs.Type);
        imageInfo.format = format;
        imageInfo.extent = { m_Metadata.Size.Width, m_Metadata.Size.Height, 1 };
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // Multi-sampling
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        VK_CHECK(vmaCreateImage(m_Allocator, &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr));

        // Create an image view for the image
        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = m_Image;
        viewInfo.viewType = VulkanUtils::ToVkImageViewType(m_Specs.Type);
        viewInfo.format = format;
        viewInfo.subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1,
        };

        VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &m_View));

        // Copy image data from buffer to image object
        VulkanStagingBuffer stagingBuffer(m_Allocator, m_Metadata.Size.Width * m_Metadata.Size.Height * Utils::ToNumOfChannels(TextureFormat::RGBA8), m_Specs.Image->Buffer.data());

        // Allocate one time command buffer
        auto cmdBuffer = VulkanUtils::AllocateCommandBuffer(m_Device, oneTimePool);

        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

        // Transition image ready for transfer
        VulkanUtils::TransitionImageLayout(
            cmdBuffer,
            m_Image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_ACCESS_2_NONE,
            VK_ACCESS_2_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_2_NONE,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT);

        // Copy the image data to the image object
        std::vector<VkBufferImageCopy> copyRegions;
        VkBufferImageCopy copyRegion = {};
        copyRegion.imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .layerCount = 1
        };
        copyRegion.imageExtent = { m_Metadata.Size.Width, m_Metadata.Size.Height, 1 },
        copyRegions.push_back(copyRegion);

        stagingBuffer.CopyToImage(cmdBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegions);

        // Transition image ready for shader reading
        VulkanUtils::TransitionImageLayout(
            cmdBuffer,
            m_Image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        VK_CHECK(vkEndCommandBuffer(cmdBuffer));

        // One time submit to populate image
        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;

        VkFence submitFence = VulkanUtils::CreateFence(m_Device, false);
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, submitFence);

        VK_CHECK(vkWaitForFences(m_Device, 1, &submitFence, VK_TRUE, UINT64_MAX));
        vkDestroyFence(m_Device, submitFence, nullptr);

        // Setup a sampler
        VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerInfo.magFilter = VulkanUtils::ToVkFilter(m_Specs.Filter);
        samplerInfo.minFilter = VulkanUtils::ToVkFilter(m_Specs.Filter);
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; // TODO
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.anisotropyEnable = m_Specs.UseAnistropicFiltering;
        samplerInfo.maxAnisotropy = m_Specs.AnisotropyLevel;

        VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler));

        // Reset image handle as we don't need it anymore
        m_Specs.Image.reset();
    }

    void VulkanTexture::Free()
    {
        if (m_Sampler)
        {
            vkDestroySampler(m_Device, m_Sampler, nullptr);
            m_Sampler = VK_NULL_HANDLE;
        }

        if (m_View)
        {
            vkDestroyImageView(m_Device, m_View, nullptr);
            m_View = VK_NULL_HANDLE;
        }

        if (m_Image)
        {
            vmaDestroyImage(m_Allocator, m_Image, m_Allocation);
            m_Image = VK_NULL_HANDLE;
            m_Allocation = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture::SetData(const void* data)
    {
        // TODO
    }

    void VulkanTexture::SetAnisotropicLevel(float level)
    {
        // TODO: recreate sampler with the specified level
        // May need to invalidate/just set m_Specs.AnisotropyLevel and require manual sampler recreation by the renderer
    }
}