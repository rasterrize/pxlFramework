#pragma once

#include <glm/vec4.hpp>

namespace pxl
{
    class RendererAPI
    {
    public:
        virtual void Clear() = 0;
        virtual void SetClearColour(const glm::vec4& colour) = 0;

        virtual void DrawArrays(uint32_t vertexCount) = 0;
        virtual void DrawLines(uint32_t vertexCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;
    };
}