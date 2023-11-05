#pragma once

#include "../RendererAPI.h"

#include <vulkan/vulkan.h>

namespace pxl
{
    class VulkanRenderer : public RendererAPI
    {
    public:
        VulkanRenderer();
        
        virtual void Clear() override;
        virtual void SetClearColour(const glm::vec4& colour) override;

        virtual void DrawArrays(uint32_t vertexCount) override;
        virtual void DrawLines(uint32_t vertexCount) override;
        virtual void DrawIndexed(uint32_t indexCount) override;
    private:
        //VkCommandBuffer
        VkClearColorValue m_ClearColour; // = { 123, 123, 123, 123 }
    };
}