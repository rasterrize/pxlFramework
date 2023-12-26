#include "VulkanRenderPass.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanRenderPass::VulkanRenderPass(VkDevice device, VkFormat format)
        : m_Device(device)
    {
        // Specify colour attachment/ref
        VkAttachmentDescription colourAttachment = {};
        colourAttachment.format = format;
        colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // used for multisampling
        colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // currently not using stencil testing
        colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // What the image layout is before the render pass begins
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // What the final image will be used for, in this case, presenting to the screen

        VkAttachmentReference colourAttachmentRef = {};
        colourAttachmentRef.attachment = 0; // references an attachment description by index in an array, in this case we have 1 attachment so we specify 0
        colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Specifies the layout of the attachment during a subpass that references this

        // Specify sub pass
        VkSubpassDescription subPass = {};
        subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments = &colourAttachmentRef; // These attachments can be referenced in the fragment shader with 'layout(location = 0) out vec4 outColor'

        // Specify sub pass dependencies
        VkSubpassDependency subPassDependency = {};
        subPassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subPassDependency.dstSubpass = 0; // our first and only subpass // must be higher than source pass
        subPassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // which stage to wait on
        subPassDependency.srcAccessMask = 0; // does this mean no mask?
        subPassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subPassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // involves writing of the color attachment, this prevents the image transition from happening until it's actually necessary. so we wait on the color attachment output stage.

        // Specify render pass
        VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colourAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subPass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &subPassDependency;

        // Create render pass
        auto result = vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass);
        VulkanHelpers::CheckVkResult(result);
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        Destroy();
    }

    void VulkanRenderPass::Destroy()
    {
        if (m_RenderPass != VK_NULL_HANDLE)
            vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    }
}