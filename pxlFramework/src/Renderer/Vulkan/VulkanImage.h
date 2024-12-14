#pragma once

#include <vulkan/vulkan.h>

#include "Core/Image.h"
#include "Core/Size.h"
#include "VulkanAllocator.h"
#include "VulkanDevice.h"

namespace pxl
{
    class VulkanImage
    {
    public:
        VulkanImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags);
        VulkanImage(Size2D size, VkFormat format, VkImageUsageFlags usageFlags)
            : VulkanImage(size.Width, size.Height, format, usageFlags)
        {
        }

        VulkanImage(const std::shared_ptr<GraphicsDevice>& device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage);

        void Destroy();

        void SetData(const Image& image);

        VkImageView GetImageView() const { return m_ImageView; }

        bool IsSwapchainImage() const { return m_IsSwapchainImage; }

    private:
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags);
        void CreateImageView(VkFormat format, VkImage image);

        void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        VkFormat ToVkFormat(ImageFormat format);

    private:
        std::shared_ptr<VulkanDevice> m_Device = nullptr;

        uint32_t m_Width = 0, m_Height = 0;
        VkFormat m_Format = VK_FORMAT_UNDEFINED;

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;

        bool m_IsSwapchainImage = false;
    };

    namespace Utils
    {
        inline uint32_t ImageFormatToBytes(ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::RGB8:      return 1 + 1 + 1;
                case ImageFormat::RGBA8:     return 1 + 1 + 1 + 1;
                case ImageFormat::Undefined: return 0;
                default:                     return 0;
            }
        }
    }
}