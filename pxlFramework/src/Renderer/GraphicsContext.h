#pragma once

#include "RendererAPIType.h"

#include <GLFW/glfw3.h>

namespace pxl
{
    class GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
        virtual void SetVSync(bool vsync) = 0;
        virtual bool GetVSync() = 0;

        static std::shared_ptr<GraphicsContext> Create(RendererAPIType api, GLFWwindow* windowHandle);
    };
}