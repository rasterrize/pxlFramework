#pragma once

#include <volk/volk.h>

#include "Renderer/GraphicsContext.h"

namespace pxl
{
    class VulkanGraphicsContext : public GraphicsContext
    {
    public:
        // Begin command buffer recording
        virtual void Begin(const std::unique_ptr<GraphicsDevice>& device) override;

        // End command buffer recording
        virtual void End(const std::unique_ptr<GraphicsDevice>& device) override;

        virtual void Bind(const std::shared_ptr<GraphicsPipeline>& pipeline) override;
        virtual void Bind(const std::shared_ptr<GPUBuffer>& buffer) override;

        // GPU draw commands
        virtual void Draw(const DrawParams& params) override;
        virtual void DrawIndexed(const DrawParams& params, const std::shared_ptr<GPUBuffer>& indexBuffer) override;

        virtual void SetClearColour(const glm::vec4& colour) override;

    private:
        void BindParams(const DrawParams& params);

        void TransitionImageLayout(
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkAccessFlags srcAccessMask,
            VkAccessFlags dstAccessMask,
            VkPipelineStageFlags2 srcStage,
            VkPipelineStageFlags2 dstStage
        );

    private:
        VkClearValue m_ClearValue;

        VkCommandBuffer m_CommandBuffer;
    };
}