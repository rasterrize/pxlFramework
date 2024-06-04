#include "VulkanFramebuffer.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanFramebuffer::VulkanFramebuffer(VkDevice device, const std::shared_ptr<VulkanRenderPass>& renderPass, VkExtent2D extent)
        : m_Device(device), m_RenderPass(renderPass->GetVKRenderPass()), m_Extent(extent)
    {
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        Destroy();
    }

    void VulkanFramebuffer::Destroy()
    {
        for (auto attachment : m_Attachments)
		{
			//vkDestroyImage(m_Device, attachment.Image, nullptr);    
			//vkDestroyImageView(m_Device, attachment.ImageView, nullptr); // TODO: move these into Image class
			//vkFreeMemory(vulkanDevice->logicalDevice, attachment.memory, nullptr);
		}
        
        if (m_Framebuffer)
        {
            vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
            m_Framebuffer = VK_NULL_HANDLE;
        }
    }

    void VulkanFramebuffer::AddAttachment(const VulkanFramebufferAttachment& attachment)
    {
        m_Attachments.push_back(attachment);
    }

    void VulkanFramebuffer::AddAttachment(VkImageView view, VkFormat format)
    { 
        m_Attachments.push_back({view, format});
    }

    void VulkanFramebuffer::Create()
    {
        std::vector<VkImageView> attachments;
        for (auto attachment : m_Attachments)
            attachments.push_back(attachment.ImageView);
        
        VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_Extent.width;
        framebufferInfo.height = m_Extent.height;
        framebufferInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffer));
    }
}