#include "VulkanRenderer.h"

#include "VulkanHelpers.h"

namespace pxl
{
    VulkanRenderer::VulkanRenderer(const std::shared_ptr<VulkanGraphicsContext>& context)
        : m_ContextHandle(context), m_Device(static_pointer_cast<VulkanDevice>(context->GetDevice())), m_GraphicsQueueFamilyIndex(m_Device->GetGraphicsQueueIndex())
    {
        // Get the graphics queue from the given queue family index
        m_GraphicsQueue = VulkanHelpers::GetQueueHandle(static_cast<VkDevice>(m_Device->GetLogical()), m_GraphicsQueueFamilyIndex);

        m_DefaultRenderPass = m_ContextHandle->GetDefaultRenderPass();

        // Set Dynamic State
        auto swapchainExtent = m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent;

        // Setup Viewport
        // NOTE: We invert the viewport here to match OpenGL - also note that this is only possible on Vulkan 1.1 or higher without extensions
        m_Viewport.x = 0.0f;
        m_Viewport.y = static_cast<float>(swapchainExtent.height);
        m_Viewport.width = static_cast<float>(swapchainExtent.width); // TODO: should width and height default to 0?
        m_Viewport.height = static_cast<float>(-(static_cast<int32_t>(swapchainExtent.height)));
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

    void VulkanRenderer::Destroy()
    {
        
    }

    void VulkanRenderer::SetViewport(uint32_t x, [[maybe_unused]] uint32_t y, uint32_t width, uint32_t height)
    {
        // Invert the given viewport values
        m_Viewport.x = static_cast<float>(x);
        m_Viewport.y = static_cast<float>(height);
        m_Viewport.width = static_cast<float>(width);
        m_Viewport.height = static_cast<float>(-(static_cast<int32_t>(height)));
    }

    void VulkanRenderer::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_Scissor.offset.x = x;
        m_Scissor.offset.y = y;
        m_Scissor.extent.width = width;
        m_Scissor.extent.height = height;
    }

    void VulkanRenderer::DrawArrays(uint32_t vertexCount)
    {
        PXL_PROFILE_SCOPE;
        
        vkCmdDraw(m_CurrentFrame.CommandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkanRenderer::DrawLines(uint32_t vertexCount)
    {
        PXL_PROFILE_SCOPE;
        
        vkCmdDraw(m_CurrentFrame.CommandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkanRenderer::DrawIndexed(uint32_t indexCount)
    {
        PXL_PROFILE_SCOPE;

        vkCmdDrawIndexed(m_CurrentFrame.CommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::Begin()
    {  
        PXL_PROFILE_SCOPE;
        
        auto device = static_cast<VkDevice>(m_Device->GetLogical());

        // Get the next frame to render to
        m_CurrentFrame = m_ContextHandle->GetSwapchain()->GetCurrentFrame();
        
        // Wait until the command buffers and semaphores are ready again
        VK_CHECK(vkWaitForFences(device, 1, &m_CurrentFrame.InFlightFence, VK_TRUE, UINT64_MAX)); // using UINT64_MAX pretty much means an infinite timeout (18 quintillion nanoseconds = 584 years)

        // Get next available image index
        m_ContextHandle->GetSwapchain()->AcquireNextAvailableImageIndex();
        uint32_t imageIndex = m_ContextHandle->GetSwapchain()->GetCurrentImageIndex();
        
        VK_CHECK(vkResetFences(device, 1, &m_CurrentFrame.InFlightFence));
        
        // Begin command buffer recording
        VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = 0; // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional - Used in secondary command buffers

        VK_CHECK(vkBeginCommandBuffer(m_CurrentFrame.CommandBuffer, &commandBufferBeginInfo));

        // --------------------------
        // Begin Geometry Render Pass
        // --------------------------

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
    }

    void VulkanRenderer::End()
    {
        PXL_PROFILE_SCOPE;
        
        // End render pass
        vkCmdEndRenderPass(m_CurrentFrame.CommandBuffer);

        // Finish recording the command buffer
        VK_CHECK(vkEndCommandBuffer(m_CurrentFrame.CommandBuffer));

        // Submit the command buffer
        VkSubmitInfo commandBufferSubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        
        VkSemaphore waitSemaphores[] = { m_CurrentFrame.ImageAvailableSemaphore }; // The semaphores to wait before execution
        VkSemaphore signalSemaphores[] = { m_CurrentFrame.RenderFinishedSemaphore };

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // which stages of the pipeline to wait on
        commandBufferSubmitInfo.waitSemaphoreCount = 1;
        commandBufferSubmitInfo.pWaitSemaphores = waitSemaphores; // semaphores to wait on before execution
        commandBufferSubmitInfo.pWaitDstStageMask = waitStages; // TODO: Understand this a little bit more
        commandBufferSubmitInfo.commandBufferCount = 1;
        commandBufferSubmitInfo.pCommandBuffers = &m_CurrentFrame.CommandBuffer;
        commandBufferSubmitInfo.signalSemaphoreCount = 1;
        commandBufferSubmitInfo.pSignalSemaphores = signalSemaphores; // semaphores to signal when finished

        m_ContextHandle->SubmitCommandBuffer(commandBufferSubmitInfo, m_GraphicsQueue, m_CurrentFrame.InFlightFence);
    }
}