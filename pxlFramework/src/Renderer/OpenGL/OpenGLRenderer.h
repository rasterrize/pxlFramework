#pragma once

#include "../RendererAPI.h"

#include "OpenGLVertexBuffer.h"
#include "OpenGLVertexArray.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLShader.h" 

namespace pxl
{
    class OpenGLRenderer : public RendererAPI
    {
    public:
        OpenGLRenderer();
        
        virtual void Clear() override;
        virtual void SetClearColour(float r, float g, float b, float a) override;

        virtual void DrawArrays(int count) override;
        virtual void DrawLines(int count) override;
        virtual void DrawIndexed() override;
    };
}