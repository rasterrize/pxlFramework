#pragma once

#include <volk/volk.h>

#include "Renderer/GraphicsContext.h"

namespace pxl
{
    class VulkanGraphicsContext : public GraphicsContext
    {
    public:
        VulkanGraphicsContext(const GraphicsContextSpecs& specs)
            : m_Specs(specs)
        {
        }

        virtual void BeginFrame(const GraphicsDevice& device, uint32_t frameIndex) override;

        virtual void EndFrame(const GraphicsDevice& device) override;

        virtual void Draw(const DrawParams& params, uint32_t vertexCount, uint32_t firstVertex = 0) override;

        virtual void DrawInstanced(const DrawParams& params, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;

        virtual void DrawIndirect(const GPUBuffer& indirectBuffer, uint64_t offset, uint32_t drawCount, uint32_t stride) override;

        virtual void DrawIndexed(const DrawParams& params, const GPUBuffer& indexBuffer, uint32_t indexCount, uint32_t firstIndex = 0, int32_t vertexOffset = 0) override;

        virtual void DrawIndexedInstanced(const DrawParams& params, const GPUBuffer& indexBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;

        virtual void DrawIndexedIndirect(const GPUBuffer& indirectBuffer, uint64_t offset, uint32_t drawCount, uint32_t stride) override;

        virtual void SetClearColour(const glm::vec4& colour) override { m_Specs.ClearColour = colour; }

    private:
        void BindParams(const DrawParams& params);

        void Bind(const GraphicsPipeline& pipeline, const GPUBuffer* uniformBuffer = nullptr);
        void Bind(const GPUBuffer& buffer);

    private:
        GraphicsContextSpecs m_Specs = {};
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    };
}