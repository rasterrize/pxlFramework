#include "OpenGLContext.h"

#include <glad/glad.h>

#include "../../Core/Window/WindowGLFW.h"

namespace pxl
{
    OpenGLContext::OpenGLContext(const std::shared_ptr<Window>& window)
    {
        m_WindowHandle = window->GetNativeWindow();
        Init();
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_GLFWWindowHandle);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            Logger::LogError("Failed to initialize Glad");
        else
            Logger::LogInfo("Glad Initialized | " + std::string("OpenGL Version: ") + std::string((const char*)glGetString(GL_VERSION)));

        glfwSwapInterval(m_VSync);
    }

    void OpenGLContext::Present()
    {
        glfwSwapBuffers(m_GLFWWindowHandle);
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