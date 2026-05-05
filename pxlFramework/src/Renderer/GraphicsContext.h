#pragma once

#include <glm/vec4.hpp>

#include "GPUBuffer.h"
#include "GraphicsDevice.h"
#include "GraphicsPipeline.h"
#include "TextureHandler.h"

namespace pxl
{
    // TODO: rename to DrawBinds?
    struct DrawParams
    {
        /// @brief Vertex buffer to bind for the draw call.
        std::shared_ptr<GPUBuffer> VertexBuffer;

        /// @brief Graphics pipeline to bind for the draw call
        std::shared_ptr<GraphicsPipeline> Pipeline;

        /// @brief (Optional) Uniform buffer to bind for the draw call.
        std::shared_ptr<GPUBuffer> UniformBuffer;
    };

    struct GraphicsContextSpecs
    {
        glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

    /// @brief Represents a context used for recording graphics operations..
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        /// @brief Begin rendering a new frame.
        virtual void BeginFrame(const GraphicsDevice& device, uint32_t frameIndex) = 0;

        /// @brief End rendering the current frame.
        virtual void EndFrame(const GraphicsDevice& device) = 0;

        /// @brief Record a non-indexed draw call.
        virtual void Draw(const DrawParams& params, uint32_t vertexCount, uint32_t firstVertex = 0) = 0;

        /// @brief Record a non-indexed instanced draw call.
        virtual void DrawInstanced(const DrawParams& params, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;

        /// @brief Record a non-indexed indirect draw call.
        virtual void DrawIndirect(const GPUBuffer& indirectBuffer, uint64_t offset, uint32_t drawCount, uint32_t stride) = 0;

        /// @brief Record an indexed draw call.
        virtual void DrawIndexed(const DrawParams& params, const GPUBuffer& indexBuffer, uint32_t indexCount, uint32_t firstIndex = 0, int32_t vertexOffset = 0) = 0;

        /// @brief Record an indexed instanced draw call.
        virtual void DrawIndexedInstanced(const DrawParams& params, const GPUBuffer& indexBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) = 0;

        /// @brief Record an indexed indirect draw call.
        virtual void DrawIndexedIndirect(const GPUBuffer& indirectBuffer, uint64_t offset, uint32_t drawCount, uint32_t stride) = 0;

        /// @brief Sets the clear colour used to clear the framebuffer at the start of rendering.
        virtual void SetClearColour(const glm::vec4& colour) = 0;
    };
}