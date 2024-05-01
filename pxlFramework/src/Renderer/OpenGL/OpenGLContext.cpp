#include "OpenGLContext.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLContext::OpenGLContext(const std::shared_ptr<Window>& window)
    {
        m_WindowHandle = window->GetNativeWindow();
        Init();
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            PXL_LOG_ERROR(LogArea::OpenGL, "Failed to initialize Glad");
        }
        else
        {
            PXL_LOG_INFO(LogArea::OpenGL, "Glad initialized - OpenGL Version: {}", (const char*)glGetString(GL_VERSION));
        }

        glfwSwapInterval(m_VSync);
    }

    void OpenGLContext::Present()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLContext::SetVSync(bool value)
    {
        if (value)
        {
            glfwSwapInterval(1);
            m_VSync = true;
        }
        else
        {
            glfwSwapInterval(0);
            m_VSync = false;
        }
    }
}