#include "VulkanGraphicsContext.h"

#include "VulkanBindlessTextureHandler.h"
#include "VulkanGPUBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsPipeline.h"

namespace pxl
{
    void VulkanGraphicsContext::BeginFrame(const GraphicsDevice& device, uint32_t frameIndex)
    {
        auto& vulkanDevice = dynamic_cast<const VulkanGraphicsDevice&>(device);

        m_CommandBuffer = vulkanDevice.GetFrameCommandBuffer(frameIndex);

        // Command buffer will only be submitted once before being recycled
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));

        // Before rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL so we can use it as a color attachment for rendering
        auto swapchainImage = vulkanDevice.GetCurrentSwapchainImage();
        VulkanUtils::TransitionImageLayout(
            m_CommandBuffer,
            swapchainImage,
            VK_IMAGE_LAYOUT_UNDEFINED,                // old layout
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // new layout
            0,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

        // Prepare color attachment
        VkClearValue clearValue;
        clearValue.color = { m_Specs.ClearColour.r, m_Specs.ClearColour.g, m_Specs.ClearColour.b, m_Specs.ClearColour.a };

        VkRenderingAttachmentInfo colorAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        colorAttachment.imageView = vulkanDevice.GetCurrentSwapchainImageView();
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = clearValue;

        // Begin rendering
        auto scExtent = vulkanDevice.GetSwapchainExtent();
        VkRenderingInfo renderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.renderArea = {
            .offset = { 0, 0 },
            .extent = scExtent,
        };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1,
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);

        // Set the viewport dynamic state
        // Flip the y coordinate to get a right-hand instead of left-hand coordinate system (similar to OpenGL)
        VkViewport viewport = {
            .x = 0.0f,
            .y = static_cast<float>(scExtent.height),
            .width = static_cast<float>(scExtent.width),
            .height = -static_cast<float>(scExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 0.0f,
        };

        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

        // Set the scissor dynamic state
        VkRect2D scissor = {
            .extent = scExtent,
        };

        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
    }

    void VulkanGraphicsContext::EndFrame(const GraphicsDevice& device)
    {
        PXL_PROFILE_SCOPE;

        vkCmdEndRendering(m_CommandBuffer);

        auto& vulkanDevice = dynamic_cast<const VulkanGraphicsDevice&>(device);

        // Transition the swapchain image to an optimal presentation layout
        auto image = vulkanDevice.GetCurrentSwapchainImage();
        VulkanUtils::TransitionImageLayout(
            m_CommandBuffer,
            image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            0,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

        VK_CHECK(vkEndCommandBuffer(m_CommandBuffer));
    }

    void VulkanGraphicsContext::Bind(const GraphicsPipeline& pipeline, const GPUBuffer& uniformBuffer, const TextureHandler* textureHandler)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(&pipeline);

        // Bind the pipeline
        auto& vulkanPipeline = dynamic_cast<const VulkanGraphicsPipeline&>(pipeline);
        VkPipeline pipelineHandle = vulkanPipeline.GetVkPipeline();
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineHandle);

        // Bind pipeline dynamic state
        auto& specs = pipeline.GetSpecs();
        vkCmdSetCullMode(m_CommandBuffer, VulkanUtils::ToVkCullMode(specs.CullMode));
        vkCmdSetFrontFace(m_CommandBuffer, VulkanUtils::ToVkFrontFace(specs.FrontFace));
        vkCmdSetPrimitiveTopology(m_CommandBuffer, VulkanUtils::ToVkPrimitiveTopology(specs.PrimitiveTopology));

        // Upload pipeline layout data
        auto layout = vulkanPipeline.GetVkPipelineLayout();
        if (pipeline.GetSpecs().UniformLayout.has_value())
        {
            auto& vulkanUniformBuffer = dynamic_cast<const VulkanGPUBuffer&>(uniformBuffer);
            auto address = vulkanUniformBuffer.GetDeviceAddress();
            vkCmdPushConstants(m_CommandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &address);
        }

        if (textureHandler)
        {
            auto vulkanBindlessHandler = dynamic_cast<const VulkanBindlessTextureHandler*>(textureHandler);
            auto set = vulkanBindlessHandler->GetDescriptorSet();
            vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &set, 0, nullptr);
        }
    }

    void VulkanGraphicsContext::Bind(const GPUBuffer& buffer)
    {
        PXL_PROFILE_SCOPE;

        auto& vulkanBuffer = dynamic_cast<const VulkanGPUBuffer&>(buffer);
        auto handle = vulkanBuffer.GetVkBuffer();
        if (vulkanBuffer.GetVkBufferUsage() & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        {
            VkDeviceSize offset = { 0 };
            // TODO: check if binding matters here
            vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &handle, &offset);
        }
        else if (vulkanBuffer.GetVkBufferUsage() & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        {
            VkDeviceSize offset = { 0 };
            vkCmdBindIndexBuffer(m_CommandBuffer, handle, offset, VK_INDEX_TYPE_UINT32);
        }
    }

    void VulkanGraphicsContext::Draw(const DrawParams& params)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(params.VertexCount > 0);
        PXL_ASSERT(params.VertexBuffer);

        BindParams(params);
        vkCmdDraw(m_CommandBuffer, params.VertexCount, 1, 0, 0);
    }

    void VulkanGraphicsContext::DrawIndexed(const DrawParams& params, const GPUBuffer& indexBuffer)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(params.IndexCount > 0);
        PXL_ASSERT(params.VertexBuffer);

        BindParams(params);
        Bind(indexBuffer);

        vkCmdDrawIndexed(m_CommandBuffer, params.IndexCount, 1, 0, 0, 0);
    }

    void VulkanGraphicsContext::SetClearColour(const glm::vec4& colour)
    {
        m_Specs.ClearColour = colour;
    }

    void VulkanGraphicsContext::BindParams(const DrawParams& params)
    {
        Bind(*params.Pipeline, *params.UniformBuffer, params.TextureHandler.get());
        Bind(*params.VertexBuffer);
    }
}