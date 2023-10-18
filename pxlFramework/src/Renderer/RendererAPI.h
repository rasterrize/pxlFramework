#pragma once

namespace pxl
{
    class RendererAPI
    {
    public:
        virtual void Clear() = 0;
        virtual void SetClearColour(float r, float g, float b, float a) = 0;

        virtual void DrawArrays(uint32_t vertexCount) = 0;
        virtual void DrawLines(uint32_t vertexCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;
    };
}