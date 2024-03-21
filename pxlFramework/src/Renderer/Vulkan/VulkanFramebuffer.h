#pragma once

#include "../Framebuffer.h"

#include "VulkanRenderPass.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    struct VulkanFramebufferAttachment
    {
        VkImageView ImageView;
        //VkImageUsageFlags Usage;
        VkFormat Format;
    };

    class VulkanFramebuffer : public Framebuffer
    {
    public:
        VulkanFramebuffer(VkDevice device, const std::shared_ptr<VulkanRenderPass>& renderPass, VkExtent2D extent);
        ~VulkanFramebuffer();

        VkFramebuffer GetVKFramebuffer() const { return m_Framebuffer; }

        virtual void Destroy() override;
        virtual void Resize(uint32_t width, uint32_t height) override { m_Extent.width = width; m_Extent.height = height; Create(); }
        void Recreate() { Create(); }
        
        void AddAttachment(const VulkanFramebufferAttachment& attachment); // return index of attachment? // could be virtual since OpenGL framebuffers are similar
        //void AddAttachment(VkImageUsageFlags usage, VkFormat format); // return index of attachment? // could be virtual since OpenGL framebuffers are similar
        void AddAttachment(VkImageView imageView, VkFormat format); // return index of attachment? // could be virtual since OpenGL framebuffers are similar
    private:
        void Create();
    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkExtent2D m_Extent = {};
        

        std::vector<VulkanFramebufferAttachment> m_Attachments;

    };
}