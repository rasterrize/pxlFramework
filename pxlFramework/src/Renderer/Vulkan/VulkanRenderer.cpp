#include "VulkanRenderer.h"

#include "VulkanHelpers.h"

#include "../../Utils/FileLoader.h"

namespace pxl
{
    VulkanRenderer::VulkanRenderer(const std::shared_ptr<VulkanContext>& context)
        : m_ContextHandle(context), m_Device(context->GetDevice()), m_GraphicsQueueFamilyIndex(context->GetGraphicsQueueIndex())
    {
        VkResult result;

        // Get the graphics queue from the given queue family index
        m_GraphicsQueue = VulkanHelpers::GetQueueHandle(m_Device, m_GraphicsQueueFamilyIndex);
        
        // Create command pool
        VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex.value();

        result = vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_CommandPool);
        VulkanHelpers::CheckVkResult(result);

        // Create command buffer
        VkCommandBufferAllocateInfo commandBufferAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocInfo.commandPool = m_CommandPool;
        commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocInfo.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(m_Device, &commandBufferAllocInfo, &m_CommandBuffer);
        VulkanHelpers::CheckVkResult(result);

        // Create sync objects
        m_InFlightFence = VulkanHelpers::CreateFence(m_Device, true); // We create it signalled so the first frame doesn't wait for an unsignallable fence

        // Create initial objects to get this bloody thing working
        auto vertBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/vert.spv");
        auto fragBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/frag.spv");

        m_Shader = pxl::Shader::Create(m_Device, vertBin, fragBin);
        m_RenderPass = m_ContextHandle->GetDefaultRenderPass();
        m_GraphicsPipeline = std::make_shared<VulkanGraphicsPipeline>(m_Device, m_Shader, m_RenderPass);

        // Set Dynamic State
        m_GraphicsPipeline->ResizeViewport(m_ContextHandle->GetSwapchain()->GetSwapchainData().Extent);
        m_GraphicsPipeline->ResizeScissor(m_ContextHandle->GetSwapchain()->GetSwapchainData().Extent);
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Destroy();
    }

    void VulkanRenderer::Destroy()
    {
        // Wait until device isnt using these objects before deleting them
        m_ContextHandle->DeviceWaitIdle();

        if (m_InFlightFence != VK_NULL_HANDLE)
            vkDestroyFence(m_Device, m_InFlightFence, nullptr);
        
        if (m_CommandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    }

    void VulkanRenderer::DrawArrays(uint32_t vertexCount)
    {
        vkCmdDraw(m_CommandBuffer, vertexCount, 1, 0, 0);
    }

    void VulkanRenderer::DrawLines(uint32_t vertexCount)
    {
        //vkCmdDraw
    }

    void VulkanRenderer::DrawIndexed(uint32_t indexCount)
    {
        vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::Begin()
    {  
        VkResult result;
        
        // Wait until the command buffers and semaphores are ready again
        vkWaitForFences(m_Device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX); // using UINT64_MAX pretty much means an infinite timeout (18 quintillion nanoseconds = 584 years)
        vkResetFences(m_Device, 1, &m_InFlightFence); // reset the fence to unsignalled state

        // Get next available image index
        m_ContextHandle->PrepareNextFrame();
        uint32_t imageIndex = m_ContextHandle->GetCurrentFrameIndex();
        
        // Begin command buffer recording
        VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = 0; // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional - Used in secondary command buffers

        result = vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo);
        VulkanHelpers::CheckVkResult(result);

        // ---------------------
        // Begin Render Pass
        // ---------------------
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.renderPass = m_RenderPass->GetVKRenderPass();
        renderPassBeginInfo.framebuffer = m_ContextHandle->GetSwapchainFramebuffer(imageIndex)->GetVKFramebuffer();
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_ContextHandle->GetSwapchain()->GetSwapchainData().Extent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &m_ClearValue;

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set dynamic state
        auto viewport = m_GraphicsPipeline->GetViewport();
        auto scissor = m_GraphicsPipeline->GetScissor();

        // Set dynamic state objects
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

        // Bind Pipeline
        m_GraphicsPipeline->Bind(m_CommandBuffer);
    }

    void VulkanRenderer::End()
    {
        VkResult result;

        // End render pass
        vkCmdEndRenderPass(m_CommandBuffer);

        // Finish recording the command buffer
        result = vkEndCommandBuffer(m_CommandBuffer);
        VulkanHelpers::CheckVkResult(result);

        // Submit the command buffer
        m_ContextHandle->SubmitCommandBuffer(m_CommandBuffer, m_GraphicsQueue, m_InFlightFence);

        m_ContextHandle->PresentReady();
    }
}