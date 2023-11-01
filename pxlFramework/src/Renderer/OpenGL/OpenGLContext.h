#pragma once

#include "../GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace pxl
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void SwapBuffers() override;
        virtual void SetVSync(bool value) override;
        virtual bool GetVSync() override { return m_VSync; }
        
    private:
        virtual void Init() override;
    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;
    };
}