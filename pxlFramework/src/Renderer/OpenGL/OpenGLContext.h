#pragma once
#include <GLFW/glfw3.h>

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

namespace pxl
{
    class OpenGLGraphicsContext : public GraphicsContext
    {
    public:
        OpenGLGraphicsContext(const std::shared_ptr<Window>& window);
        virtual ~OpenGLGraphicsContext() override = default;

        virtual void Present() override;

        virtual bool GetVSync() const override { return m_VSync; }
        virtual void SetVSync(bool value) override;
        virtual void ToggleVSync() override { SetVSync(!m_VSync); }

        virtual void SetAsCurrent() override;

        virtual std::shared_ptr<GraphicsDevice> GetDevice() const override
        {
            PXL_LOG_ERROR(LogArea::OpenGL, "OpenGLContexts don't have devices, returning nullptr");
            return nullptr;
        }

        virtual RendererLimits GetLimits() override;

    private:
        GLFWwindow* m_GLFWWindowHandle = nullptr;
        bool m_VSync = true;
    };
}