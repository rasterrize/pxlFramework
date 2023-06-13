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
            Logger::LogInfo("Successfully initialized Glad");

        glfwSwapInterval(1);
        m_VSync = true;
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    void OpenGLContext::SetVSync(bool vsync)
    {
        if (vsync)
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