#pragma once

#include "GPUBuffer.h"
#include "GraphicsPipeline.h"
#include "GraphicsDevice.h"

namespace pxl
{
    struct DrawParams
    {
        std::vector<std::shared_ptr<GPUBuffer>> VertexBuffers;
        std::shared_ptr<GraphicsPipeline> Pipeline = nullptr;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;
    };

    /// @brief Represents a context used for recording graphics operations.
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Begin(const std::unique_ptr<GraphicsDevice>& device) = 0;
        virtual void End(const std::unique_ptr<GraphicsDevice>& device) = 0;

        virtual void Bind(const std::shared_ptr<GraphicsPipeline>& pipeline) = 0;
        virtual void Bind(const std::shared_ptr<GPUBuffer>& buffer) = 0;

        virtual void Draw(const DrawParams& params) = 0;
        virtual void DrawIndexed(const DrawParams& params, const std::shared_ptr<GPUBuffer>& indexBuffer) = 0;

        virtual void SetClearColour(const glm::vec4& colour) = 0;
    };
}