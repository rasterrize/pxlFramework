#include "OpenGLContext.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        Init();
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            Logger::LogError("Failed to initialize Glad");
        else
            Logger::LogInfo("Glad Initialized");
            Logger::LogInfo("- OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)));

        glfwSwapInterval(m_VSync);
    }

    void OpenGLContext::SwapBuffers()
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