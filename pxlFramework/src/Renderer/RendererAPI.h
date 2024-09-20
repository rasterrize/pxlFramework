#pragma once

#include <glm/vec4.hpp>

#include "Core/Window.h"
#include "RendererAPIType.h"

namespace pxl
{
    class RendererAPI
    {
    public:
        virtual ~RendererAPI() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void Clear() = 0;
        virtual void SetClearColour(const glm::vec4& colour) = 0;

        virtual void DrawArrays(uint32_t vertexCount) = 0;
        virtual void DrawLines(uint32_t vertexCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        static std::unique_ptr<RendererAPI> Create(RendererAPIType api, const std::shared_ptr<Window>& window);
    };
}