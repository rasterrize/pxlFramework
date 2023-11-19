#pragma once

#include "../GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace pxl
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Present() override;
        virtual void SetVSync(bool value) override;
        virtual bool GetVSync() override { return m_VSync; }
        
    private:
        void Init();
    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;
    };
}