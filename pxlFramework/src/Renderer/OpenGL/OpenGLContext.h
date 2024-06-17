#pragma once

#include "../GraphicsContext.h"

#include <GLFW/glfw3.h>

#include "../../Core/Window.h"

namespace pxl
{
    class OpenGLGraphicsContext : public GraphicsContext
    {
    public:
        OpenGLGraphicsContext(const std::shared_ptr<Window>& window);
        virtual ~OpenGLGraphicsContext() override {};

        virtual void Present() override;

        virtual void SetVSync(bool value) override;
        virtual bool GetVSync() const override { return m_VSync; }

        virtual std::shared_ptr<Device> GetDevice() const override { return nullptr; }
        
    private:
        void Init();
    private:
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_VSync = true;
    };
}