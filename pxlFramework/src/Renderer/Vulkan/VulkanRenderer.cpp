#include "VulkanRenderer.h"

#include "VulkanHelpers.h"

#include "../../Utils/FileLoader.h"

//temp
#include "../Renderer.h"

namespace pxl
{
    VulkanRenderer::VulkanRenderer(const std::shared_ptr<VulkanContext>& context)
        : m_ContextHandle(context), m_Device(context->GetDevice()), m_GraphicsQueueFamilyIndex(m_Device->GetGraphicsQueueIndex())
    {
        VkResult result;

        auto logicalDevice = m_Device->GetVkDevice();

        // Get the graphics queue from the given queue family index
        m_GraphicsQueue = VulkanHelpers::GetQueueHandle(logicalDevice, m_GraphicsQueueFamilyIndex);

        m_DefaultRenderPass = m_ContextHandle->GetDefaultRenderPass();

        // Set Dynamic State
        auto swapchainExtent = m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent;

        // Setup Viewport
        m_Viewport.x = 0.0f;
        m_Viewport.y = 0.0f;
        m_Viewport.width = swapchainExtent.width; // TODO: should width and height default to 0?
        m_Viewport.height = swapchainExtent.height;
        m_Viewport.minDepth = 0.0f;
        m_Viewport.maxDepth = 1.0f;

        // Setup Scissor
        m_Scissor.offset = { 0, 0 };
        m_Scissor.extent = { swapchainExtent.width, swapchainExtent.height };
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Destroy();
    }

    void VulkanRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_Viewport.x = x;
        m_Viewport.y = y;
        m_Viewport.width = width;
        m_Viewport.height = height;
    }

    void VulkanRenderer::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_Scissor.offset.x = x;
        m_Scissor.offset.y = y;
        m_Scissor.extent.width = width;
        m_Scissor.extent.height = height;
    }

    void VulkanRenderer::Destroy()
    {
    }

    void VulkanRenderer::DrawArrays(uint32_t vertexCount)
    {
        vkCmdDraw(m_CurrentFrame.CommandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkanRenderer::DrawLines(uint32_t vertexCount)
    {
        //vkCmdDraw
    }

    void VulkanRenderer::DrawIndexed(uint32_t indexCount)
    {
        vkCmdDrawIndexed(m_CurrentFrame.CommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::Begin()
    {  
        VkResult result;

        // Get the next frame to render to
        m_CurrentFrame = m_ContextHandle->GetCurrentFrame();
        
        // Wait until the command buffers and semaphores are ready again
        vkWaitForFences(m_Device->GetVkDevice(), 1, &m_CurrentFrame.InFlightFence, VK_TRUE, UINT64_MAX); // using UINT64_MAX pretty much means an infinite timeout (18 quintillion nanoseconds = 584 years)

        // Get next available image index
        m_ContextHandle->AcquireNextImage();
        uint32_t imageIndex = m_ContextHandle->GetCurrentFrameIndex();
        
        vkResetFences(m_Device->GetVkDevice(), 1, &m_CurrentFrame.InFlightFence); // reset the fence to unsignalled state
        
        // Begin command buffer recording
        VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = 0; // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional - Used in secondary command buffers

        result = vkBeginCommandBuffer(m_CurrentFrame.CommandBuffer, &commandBufferBeginInfo);
        VulkanHelpers::CheckVkResult(result);

        // ---------------------
        // Begin Geometry Render Pass
        // ---------------------
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.renderPass = m_DefaultRenderPass->GetVKRenderPass(); // shouldnt be here if I want to support different render passes in the future.
        renderPassBeginInfo.framebuffer = m_ContextHandle->GetSwapchain()->GetFramebuffer(imageIndex)->GetVKFramebuffer();
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &m_ClearValue;

        vkCmdBeginRenderPass(m_CurrentFrame.CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set dynamic state objects
        vkCmdSetViewport(m_CurrentFrame.CommandBuffer, 0, 1, &m_Viewport);
        vkCmdSetScissor(m_CurrentFrame.CommandBuffer, 0, 1, &m_Scissor);

        // Bind Pipeline (from application)
    }

    void VulkanRenderer::End()
    {
        VkResult result;

        // End render pass
        vkCmdEndRenderPass(m_CurrentFrame.CommandBuffer);

        // Finish recording the command buffer
        result = vkEndCommandBuffer(m_CurrentFrame.CommandBuffer);
        VulkanHelpers::CheckVkResult(result);

        // Submit the command buffer
        m_ContextHandle->SubmitCommandBuffer(m_CurrentFrame.CommandBuffer, m_GraphicsQueue, m_CurrentFrame.InFlightFence);
    }
}