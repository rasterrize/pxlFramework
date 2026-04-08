#pragma once

#include "GPUBuffer.h"
#include "GraphicsDevice.h"
#include "GraphicsPipeline.h"
#include "TextureHandler.h"

namespace pxl
{
    struct DrawParams
    {
        std::vector<std::shared_ptr<GPUBuffer>> VertexBuffers;
        std::shared_ptr<GraphicsPipeline> Pipeline = nullptr;
        std::shared_ptr<GPUBuffer> UniformBuffer = nullptr;

        std::shared_ptr<TextureHandler> TextureHandler = nullptr;

        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;

        // TODO: extend with vertex/index offsets, instance count, etc
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
        virtual void BeginFrame(const std::unique_ptr<GraphicsDevice>& device, uint32_t frameIndex) = 0;

        /// @brief End rendering the current frame.
        virtual void EndFrame(const std::unique_ptr<GraphicsDevice>& device) = 0;

        /// @brief Binds a GraphicsPipeline to the current frames state.
        virtual void Bind(const std::shared_ptr<GraphicsPipeline>& pipeline, const std::shared_ptr<GPUBuffer>& uniformBuffer, const std::shared_ptr<TextureHandler>& textureHandler) = 0;

        /// @brief Binds a GPUBuffer to the current frames state.
        virtual void Bind(const std::shared_ptr<GPUBuffer>& buffer) = 0;

        /// @brief Records a non-indexed draw call.
        virtual void Draw(const DrawParams& params) = 0;

        /// @brief Records an indexed draw call.
        virtual void DrawIndexed(const DrawParams& params, const std::shared_ptr<GPUBuffer>& indexBuffer) = 0;

        /// @brief Sets the clear colour used when a new frame begins rendering.
        virtual void SetClearColour(const glm::vec4& colour) = 0;


    };
}