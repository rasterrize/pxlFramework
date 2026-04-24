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

        // Begin recording commands for a new frame of the specified frameIndex
        virtual void BeginFrame(const GraphicsDevice& device, uint32_t frameIndex) override;

        // End recording commands of the current frame
        virtual void EndFrame(const GraphicsDevice& device) override;

        virtual void Bind(const GraphicsPipeline& pipeline, const GPUBuffer& uniformBuffer, const TextureHandler* textureHandler) override;
        virtual void Bind(const std::shared_ptr<GPUBuffer>& buffer) override;

        // GPU draw commands
        virtual void Draw(const DrawParams& params) override;
        virtual void DrawIndexed(const DrawParams& params, const std::shared_ptr<GPUBuffer>& indexBuffer) override;

        virtual void SetClearColour(const glm::vec4& colour) override;

    private:
        void BindParams(const DrawParams& params);

    private:
        GraphicsContextSpecs m_Specs = {};
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    };
}