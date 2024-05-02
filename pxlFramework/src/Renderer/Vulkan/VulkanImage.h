#pragma once

#include <vulkan/vulkan.h>

namespace pxl
{
    // struct ImageSpec
    // {
    //     uint32_t Width = 0;
    //     uint32_t Height = 0;
    //     ImageFormat Format = ImageFormat::Undefined;
    // };

    // TODO: abstracted
    class VulkanImage
    {
    public:
        VulkanImage(VkDevice device, uint32_t width, uint32_t height, VkFormat format);
        VulkanImage(VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImage swapchainImage);
        ~VulkanImage();

        void Destroy();

        VkImageView GetImageView() const { return m_ImageView; }
    private:
        void CreateImage(uint32_t width, uint32_t height, VkFormat format);
        void CreateImageView(uint32_t width, uint32_t height, VkFormat format, VkImage image);
    private:
        uint32_t m_Width = 0, m_Height = 0;
        VkFormat m_Format = VK_FORMAT_UNDEFINED;

        VkImage m_Image = VK_NULL_HANDLE;
        VkImageView m_ImageView = VK_NULL_HANDLE;

        bool m_IsSwapchainImage = false; // Currently unused

        // for destruction
        VkDevice m_Device = VK_NULL_HANDLE;
    };
}