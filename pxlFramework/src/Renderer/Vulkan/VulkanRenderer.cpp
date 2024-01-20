#include "VulkanRenderer.h"

#include "VulkanHelpers.h"

#include "../../Utils/FileLoader.h"

//temp
#include "../BufferLayout.h"

namespace pxl
{
    VulkanRenderer::VulkanRenderer(const std::shared_ptr<VulkanContext>& context)
        : m_ContextHandle(context), m_Device(context->GetDevice()), m_GraphicsQueueFamilyIndex(context->GetGraphicsQueueIndex())
    {
        VkResult result;

        // Get the graphics queue from the given queue family index
        m_GraphicsQueue = VulkanHelpers::GetQueueHandle(m_Device, m_GraphicsQueueFamilyIndex);

        // Create initial objects to get this bloody thing working
        auto vertBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/vert.spv");
        auto fragBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/frag.spv");

        m_Shader = pxl::Shader::Create(m_Device, vertBin, fragBin);
        m_RenderPass = m_ContextHandle->GetDefaultRenderPass();

        BufferLayout layout;
        layout.Add(1, BufferDataType::Float2, false);

        m_GraphicsPipeline = std::make_shared<VulkanGraphicsPipeline>(m_Device, m_Shader, m_RenderPass, layout);

        float vertices[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f 
        };

        uint32_t indices[] = {
            0, 1, 2, 2, 3, 0
        };

        m_TestVertexBuffer = std::make_shared<VulkanBuffer>(m_ContextHandle->GetPhysicalDevice(), m_Device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, (uint32_t)sizeof(vertices), vertices);
        m_TestIndexBuffer = std::make_shared<VulkanBuffer>(m_ContextHandle->GetPhysicalDevice(), m_Device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, (uint32_t)sizeof(indices), indices);

        // Set Dynamic State
        auto swapchainExtent = m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent;

        // VkViewport viewport = {};
        // viewport.x = 0.0f;
        // viewport.y = 540.0f;
        // viewport.width = 1920.0f;
        // viewport.height = 1080.0f;
        // viewport.minDepth = 0.0f;
        // viewport.maxDepth = 1.0f;

        m_GraphicsPipeline->ResizeViewport(m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent);
        m_GraphicsPipeline->ResizeScissor(m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent);
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Destroy();
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
        vkWaitForFences(m_Device, 1, &m_CurrentFrame.InFlightFence, VK_TRUE, UINT64_MAX); // using UINT64_MAX pretty much means an infinite timeout (18 quintillion nanoseconds = 584 years)

        // Get next available image index
        m_ContextHandle->AcquireNextImage();
        uint32_t imageIndex = m_ContextHandle->GetCurrentFrameIndex();
        
        vkResetFences(m_Device, 1, &m_CurrentFrame.InFlightFence); // reset the fence to unsignalled state
        
        // Begin command buffer recording
        VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = 0; // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional - Used in secondary command buffers

        result = vkBeginCommandBuffer(m_CurrentFrame.CommandBuffer, &commandBufferBeginInfo);
        VulkanHelpers::CheckVkResult(result);

        // ---------------------
        // Begin Render Pass
        // ---------------------
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.renderPass = m_RenderPass->GetVKRenderPass();
        renderPassBeginInfo.framebuffer = m_ContextHandle->GetSwapchain()->GetFramebuffer(imageIndex)->GetVKFramebuffer();
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_ContextHandle->GetSwapchain()->GetSwapchainSpecs().Extent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &m_ClearValue;

        vkCmdBeginRenderPass(m_CurrentFrame.CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set dynamic state
        auto viewport = m_GraphicsPipeline->GetViewport();
        auto scissor = m_GraphicsPipeline->GetScissor();

        // Set dynamic state objects
        vkCmdSetViewport(m_CurrentFrame.CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(m_CurrentFrame.CommandBuffer, 0, 1, &scissor);

        // Bind Pipeline
        m_GraphicsPipeline->Bind(m_CurrentFrame.CommandBuffer);

        // Temp
        m_TestVertexBuffer->Bind(m_CurrentFrame.CommandBuffer);
        m_TestIndexBuffer->Bind(m_CurrentFrame.CommandBuffer);
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