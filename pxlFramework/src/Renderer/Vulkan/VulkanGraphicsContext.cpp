#include "VulkanGraphicsContext.h"

#include "VulkanGPUBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsPipeline.h"

namespace pxl
{
    void VulkanGraphicsContext::Begin(const std::unique_ptr<GraphicsDevice>& device)
    {
        VulkanGraphicsDevice* vulkanDevice = dynamic_cast<VulkanGraphicsDevice*>(device.get());
        vulkanDevice->AcquireNextSwapchainImage();
        m_CommandBuffer = vulkanDevice->GetCurrentFrame().CommandBuffer;

        // Command buffer will only be submitted once before being recycled
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));

        // Before rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL so we can use it as a color attachment
        auto swapchainImage = vulkanDevice->GetCurrentSwapchainImage();
        TransitionImageLayout(
            swapchainImage,
            VK_IMAGE_LAYOUT_UNDEFINED,                // old layout
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // new layout
            0,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

        // Prepare color attachment
        VkRenderingAttachmentInfo colorAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        colorAttachment.imageView = vulkanDevice->GetCurrentSwapchainImageView();
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = m_ClearValue;

        // Begin rendering
        auto scExtent = vulkanDevice->GetSwapchainExtent();
        VkRenderingInfo renderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.renderArea = {
            .offset = { 0, 0 },
            .extent = scExtent,
        };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1,
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);

        // Set global dynamic state

        // Set viewport dynamically
        // NOTE: We flip the y coordinate to get a right-hand instead of left-hand coordinate system (similar to OpenGL)
        VkViewport viewport = {
            .x = 0.0f,
            .y = static_cast<float>(scExtent.height),
            .width = static_cast<float>(scExtent.width),
            .height = -static_cast<float>(scExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 0.0f,
        };

        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

        // Set scissor dynamically
        VkRect2D scissor = {
            .extent = scExtent,
        };

        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
    }

    void VulkanGraphicsContext::End(const std::unique_ptr<GraphicsDevice>& device)
    {
        vkCmdEndRendering(m_CommandBuffer);

        VulkanGraphicsDevice* vulkanDevice = dynamic_cast<VulkanGraphicsDevice*>(device.get());

        auto image = vulkanDevice->GetCurrentSwapchainImage();
        TransitionImageLayout(
            image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            0,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

        VK_CHECK(vkEndCommandBuffer(m_CommandBuffer));

        vulkanDevice->SubmitCurrentFrame();
    }

    void VulkanGraphicsContext::Bind(const std::shared_ptr<GraphicsPipeline>& pipeline, const std::shared_ptr<GPUBuffer>& uniformBuffer)
    {
        // Bind the pipeline
        auto vulkanPipeline = static_pointer_cast<VulkanGraphicsPipeline>(pipeline);
        auto pipelineHandle = vulkanPipeline->GetVkPipeline();
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineHandle);
        
        // Bind pipeline dynamic state
        auto specs = pipeline->GetSpecs();
        vkCmdSetCullMode(m_CommandBuffer, VulkanUtils::ToVkCullMode(specs.CullMode));
        vkCmdSetFrontFace(m_CommandBuffer, VulkanUtils::ToVkFrontFace(specs.FrontFace));
        vkCmdSetPrimitiveTopology(m_CommandBuffer, VulkanUtils::ToVkPrimitiveTopology(specs.PrimitiveTopology));
        
        // Upload pipeline layout data
        if (pipeline->GetSpecs().UniformLayout.has_value())
        {
            auto layout = vulkanPipeline->GetVkPipelineLayout();
            auto vulkanUniformBuffer = static_pointer_cast<VulkanGPUBuffer>(uniformBuffer);
            auto address = vulkanUniformBuffer->GetDeviceAddress();
            vkCmdPushConstants(m_CommandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &address);
        }

        m_Stats.PipelineBinds++;
    }

    void VulkanGraphicsContext::Bind(const std::shared_ptr<GPUBuffer>& buffer)
    {
        auto vulkanBuffer = std::static_pointer_cast<VulkanGPUBuffer>(buffer);
        auto handle = vulkanBuffer->GetVkBuffer();
        if (vulkanBuffer->GetVkBufferUsage() & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        {
            VkDeviceSize offset = { 0 };
            // TODO: check if binding matters here
            vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &handle, &offset);
        }
        else if (vulkanBuffer->GetVkBufferUsage() & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        {
            VkDeviceSize offset = { 0 };
            vkCmdBindIndexBuffer(m_CommandBuffer, handle, offset, VK_INDEX_TYPE_UINT32);
        }
    }

    void VulkanGraphicsContext::Draw(const DrawParams& params)
    {
        BindParams(params);
        vkCmdDraw(m_CommandBuffer, params.VertexCount, 1, 0, 0);
    }

    void VulkanGraphicsContext::DrawIndexed(const DrawParams& params, const std::shared_ptr<GPUBuffer>& indexBuffer)
    {
        BindParams(params);
        Bind(indexBuffer);

        // auto pipelineLayout = static_pointer_cast<VulkanGraphicsPipeline>(params.Pipeline)->GetVkPipelineLayout();
        // vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,

        vkCmdDrawIndexed(m_CommandBuffer, params.IndexCount, 1, 0, 0, 0);
    }

    void VulkanGraphicsContext::SetClearColour(const glm::vec4& colour)
    {
        m_ClearValue.color = { colour.r, colour.g, colour.b, colour.a };
    }

    void VulkanGraphicsContext::BindParams(const DrawParams& params)
    {
        Bind(params.Pipeline, params.UniformBuffer);

        for (const auto& buffer : params.VertexBuffers)
            Bind(buffer);
    }

    void VulkanGraphicsContext::TransitionImageLayout(
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        VkPipelineStageFlags2 srcStage,
        VkPipelineStageFlags2 dstStage)
    {
        VkImageMemoryBarrier2 imageBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        imageBarrier.srcStageMask = srcStage;
        imageBarrier.srcAccessMask = srcAccessMask;
        imageBarrier.dstStageMask = dstStage;
        imageBarrier.dstAccessMask = dstAccessMask;
        imageBarrier.oldLayout = oldLayout;
        imageBarrier.newLayout = newLayout;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image = image;
        imageBarrier.subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        dependencyInfo.dependencyFlags = 0; // No special dependency flags
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageBarrier;

        vkCmdPipelineBarrier2(m_CommandBuffer, &dependencyInfo);
    }
}