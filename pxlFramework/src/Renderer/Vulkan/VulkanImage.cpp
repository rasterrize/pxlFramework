#include "VulkanImage.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanImage::VulkanImage(const std::shared_ptr<VulkanDevice>& device, uint32_t width, uint32_t height, VkFormat format)
        : m_Device(static_cast<VkDevice>(device->GetLogical())), m_Width(width), m_Height(height), m_Format(format)
    {
        // Create a vulkan image AND image view
        CreateImage(m_Width, m_Height, m_Format);
        CreateImageView(m_Format, m_Image);
    }

    VulkanImage::VulkanImage(const std::shared_ptr<VulkanDevice>& device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage)
        : m_Device(static_cast<VkDevice>(device->GetLogical())), m_Width(width), m_Height(height), m_Format(format), m_IsSwapchainImage(true)
    {
        // Create just a vulkan image view for the supplied image
        CreateImageView(m_Format, swapchainImage);
    }

    void VulkanImage::Destroy()
    {
        if (m_ImageView)
        {
            vkDestroyImage(m_Device, m_Image, nullptr);
            m_Image = VK_NULL_HANDLE;
        }

        if (m_ImageView)
        {
            vkDestroyImageView(m_Device, m_ImageView, nullptr);
            m_ImageView = VK_NULL_HANDLE;
        }
    }

    void VulkanImage::CreateImage([[maybe_unused]] uint32_t width, [[maybe_unused]] uint32_t height, [[maybe_unused]] VkFormat format)
    {
        // TODO: Create VkImage object
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
}