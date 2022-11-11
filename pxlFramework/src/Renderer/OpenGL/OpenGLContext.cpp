#include "OpenGLContext.h"
#include "../../Core/Window.h"
#include <glad/glad.h>

namespace pxl
{
    OpenGLContext::OpenGLContext()
    {
        m_WindowHandle = Window::GetNativeWindow();
        Init();
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize Glad");
        }
        else
        {
            Logger::LogInfo("Successfully initialized Glad");
        }
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}