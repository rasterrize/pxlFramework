#pragma once

#include "GraphicsDevice.h"
#include "RendererAPIType.h"
#include "RendererLimits.h"

namespace pxl
{
    // Use forward declaration instead of include to avoid cycling includes
    class Window;

    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Present() = 0;

        virtual bool GetVSync() const = 0;
        virtual void SetVSync(bool value) = 0;
        virtual void ToggleVSync() = 0;

        // Required for OpenGL
        virtual void SetAsCurrent() = 0;

        virtual std::shared_ptr<GraphicsDevice> GetDevice() const = 0;

        virtual RendererLimits GetLimits() = 0;

        static std::shared_ptr<GraphicsContext> Create(RendererAPIType api, const std::shared_ptr<Window>& window);
    };
}