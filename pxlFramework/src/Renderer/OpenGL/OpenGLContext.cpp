#include "OpenGLContext.h"
#include "../../Core/Window.h"
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
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}