#pragma once

#include "../GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace pxl
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext();

        virtual void SwapBuffers() override;
    private:
        virtual void Init() override;
        GLFWwindow* m_WindowHandle;
    };
}