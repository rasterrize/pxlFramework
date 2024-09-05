#include "OpenGLContext.h"

#include <glad/glad.h>

namespace pxl
{
    static void GLCallback([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id,
        [[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei length, [[maybe_unused]] const GLchar* message, [[maybe_unused]] const void* userParam)
    {
        PXL_LOG_ERROR(LogArea::OpenGL, message);
    }

    OpenGLGraphicsContext::OpenGLGraphicsContext(const std::shared_ptr<Window>& window)
        : m_WindowHandle(window->GetNativeWindow())
    {
        Init();
    }

    void OpenGLGraphicsContext::Init()
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

        glDebugMessageCallback(GLCallback, nullptr);

        glfwSwapInterval(m_VSync);
    }

    void OpenGLGraphicsContext::Present()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLGraphicsContext::SetVSync(bool value)
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