#pragma once

#include "Core/Window.h"
#include "Renderer/RendererAPI.h"

namespace pxl
{
    class OpenGLRenderer : public RendererAPI
    {
    public:
        OpenGLRenderer();

        virtual void BeginFrame() override {};
        virtual void EndFrame() override {};

        virtual void Clear() override;
        virtual void SetClearColour(const glm::vec4& colour) override;

        virtual void DrawArrays(uint32_t vertexCount) override;
        virtual void DrawLines(uint32_t vertexCount) override;
        virtual void DrawIndexed(uint32_t indexCount) override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    private:
        bool m_ScissorEnabled = false;
    };
}