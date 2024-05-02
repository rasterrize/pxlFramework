#pragma once

#include <glm/vec4.hpp>

#include "Buffer.h"

namespace pxl
{
    class RendererAPI
    {
    public:
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Clear() = 0;
        virtual void SetClearColour(const glm::vec4& colour) = 0;

        virtual void DrawArrays(uint32_t vertexCount) = 0;
        virtual void DrawLines(uint32_t vertexCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;

        // virtual void Bind(const std::shared_ptr<Buffer>& buffer) // could automatically determine the buffer type in source and bind it that way, which is technically what we currently do

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    };
}