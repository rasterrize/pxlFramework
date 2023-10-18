#pragma once

#include "../RendererAPI.h"

namespace pxl
{
    class OpenGLRenderer : public RendererAPI
    {
    public:
        OpenGLRenderer();
        
        virtual void Clear() override;
        virtual void SetClearColour(float r, float g, float b, float a) override;

        virtual void DrawArrays(uint32_t vertexCount) override;
        virtual void DrawLines(uint32_t vertexCount) override;
        virtual void DrawIndexed(uint32_t indexCount) override;
    };
}