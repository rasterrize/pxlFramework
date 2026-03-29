#pragma once

#include "GPUBuffer.h"
#include "GraphicsDevice.h"
#include "GraphicsPipeline.h"

namespace pxl
{
    struct DrawParams
    {
        std::vector<std::shared_ptr<GPUBuffer>> VertexBuffers;
        std::shared_ptr<GraphicsPipeline> Pipeline = nullptr;
        std::shared_ptr<GPUBuffer> UniformBuffer = nullptr;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;
    };

    struct GraphicsContextStats
    {
        uint32_t DrawCalls;
        uint32_t PipelineBinds;
        uint32_t VertexBufferBinds;
        uint32_t IndexBufferBinds;
        // uint32_t UniformBufferBinds;
    };

    struct GraphicsContextSpecs
    {
        glm::vec4 ClearColour;
    };

    /// @brief Represents a context used for recording graphics operations..
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        /// @brief Begin rendering a new frame.
        virtual void Begin(const std::unique_ptr<GraphicsDevice>& device) = 0;

        /// @brief End rendering the current frame.
        virtual void End(const std::unique_ptr<GraphicsDevice>& device) = 0;

        /// @brief Binds a GraphicsPipeline to the current frames state.
        virtual void Bind(const std::shared_ptr<GraphicsPipeline>& pipeline, const std::shared_ptr<GPUBuffer>& uniformBuffer) = 0;

        /// @brief Binds a GPUBuffer to the current frames state.
        virtual void Bind(const std::shared_ptr<GPUBuffer>& buffer) = 0;

        /// @brief Records a non-indexed (no index buffer) draw call.
        virtual void Draw(const DrawParams& params) = 0;

        /// @brief Records an indexed draw call.
        virtual void DrawIndexed(const DrawParams& params, const std::shared_ptr<GPUBuffer>& indexBuffer) = 0;

        /// @brief Sets the clear colour used when a new frame begins rendering.
        virtual void SetClearColour(const glm::vec4& colour) = 0;

        GraphicsContextStats GetStats() const { return m_Stats; }

    protected:
        GraphicsContextStats m_Stats;
    };
}