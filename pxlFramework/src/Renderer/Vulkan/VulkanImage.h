#pragma once

#include <vulkan/vulkan.h>

#include "Core/Image.h"
#include "Core/Size.h"
#include "VulkanAllocator.h"
#include "VulkanDevice.h"

namespace pxl
{
    // TODO: abstracted
    class VulkanImage
    {
    public:
        VulkanImage(uint32_t width, uint32_t height, VkFormat format);
        VulkanImage(const std::shared_ptr<VulkanDevice>& device, uint32_t width, uint32_t height, VkFormat format);
        VulkanImage(const std::shared_ptr<VulkanDevice>& device, Size2D size, VkFormat format);
        VulkanImage(const std::shared_ptr<VulkanDevice>& device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage);

        void Destroy();

        void SetData(const Image& image);

        VkImageView GetImageView() const { return m_ImageView; }

    private:
        void CreateImage(uint32_t width, uint32_t height, VkFormat format);
        void CreateImageView(VkFormat format, VkImage image);

        VkFormat ToVkFormat(ImageFormat format);
    private:
        VkDevice m_Device = VK_NULL_HANDLE;

        uint32_t m_Width = 0, m_Height = 0;
        VkFormat m_Format = VK_FORMAT_UNDEFINED;

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;
        VmaAllocation m_Allocation = VK_NULL_HANDLE;

        bool m_IsSwapchainImage = false; // Currently unused

    };
}