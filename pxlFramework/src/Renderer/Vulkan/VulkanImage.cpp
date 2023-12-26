#include "VulkanImage.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanImage::VulkanImage(VkDevice device, uint32_t width, uint32_t height, VkFormat format)
        : m_Device(device), m_Width(width), m_Height(height), m_Format(format)
    {
        // Create a vulkan image AND image view
        CreateImage(m_Width, m_Height, m_Format);
        CreateImageView(m_Width, m_Height, m_Format, m_Image);
    }

    VulkanImage::VulkanImage(VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage)
        : m_Device(device), m_Width(width), m_Height(height), m_Format(format), m_IsSwapchainImage(true)
    {
        // Create just a vulkan image view for the supplied image
        CreateImageView(m_Width, m_Height, m_Format, swapchainImage);
    }

    VulkanImage::~VulkanImage()
    {
        if (m_Image != VK_NULL_HANDLE)
            vkDestroyImage(m_Device, m_Image, nullptr);

        if (m_ImageView != VK_NULL_HANDLE)
            vkDestroyImageView(m_Device, m_ImageView, nullptr);
    }

    void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format)
    {
        // TODO: Create VkImage object
    }

    void VulkanImage::CreateImageView(uint32_t width, uint32_t height, VkFormat format, VkImage image)
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

        auto result = vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_ImageView);
        VulkanHelpers::CheckVkResult(result);
    }
}