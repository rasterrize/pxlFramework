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
            Logger::LogError("Failed to initialize Glad");
        else
            Logger::LogInfo("Glad Initialized | " + std::string("OpenGL Version: ") + std::string((const char*)glGetString(GL_VERSION)));

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

    void OpenGLContext::ResizeViewport(uint32_t width, uint32_t height)
    {
        glViewport(0, 0, width, height);
    }
}