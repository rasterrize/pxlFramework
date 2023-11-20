#pragma once

#include "../GraphicsContext.h"
#include "../../Core/Window.h"

#include <GLFW/glfw3.h>

namespace pxl
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(const std::shared_ptr<Window> window);

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