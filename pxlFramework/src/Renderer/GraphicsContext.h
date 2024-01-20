#pragma once

#include "RendererAPIType.h"

namespace pxl
{
    class Window;

    class GraphicsContext
    {
    public:
        virtual void Present() = 0;
        virtual void SetVSync(bool value) = 0;
        virtual bool GetVSync() = 0;
        virtual void ResizeViewport(uint32_t width, uint32_t height) = 0;

        static std::shared_ptr<GraphicsContext> Create(RendererAPIType api, const std::shared_ptr<Window>& window);
    };
}