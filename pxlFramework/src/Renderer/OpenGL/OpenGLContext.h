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
        virtual void SetVSync(bool vsync) override;
        virtual bool GetVSync() override { return m_VSync; }

        virtual void* GetNativeContext() override { return nullptr; }
        virtual void* GetDevice() override { return nullptr; }
        virtual void* GetBackBuffer() override { return nullptr; }
    private:
        virtual void Init() override;
    private:
        GLFWwindow* m_WindowHandle;
        bool m_VSync = true;
    };
}