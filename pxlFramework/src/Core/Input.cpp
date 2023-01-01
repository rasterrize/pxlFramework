#include "Input.h"
#include "Window.h"
#include "Application.h"

namespace pxl
{
    GLFWwindow* Input::s_WindowHandle;
    bool Input::s_Enabled = false;

    void Input::Init()
    {
        if (s_Enabled)
        {
            Logger::LogWarn("Failed to initalize input, it's already initialized");
            return;
        }
        
        if (Application::Get().IsRunning());
        {
            Logger::LogError("Can't intialize input, no application exists");
            return;
        }

        s_WindowHandle = Window::GetNativeWindow();
        if (!s_WindowHandle)
        {
            Logger::Log(LogLevel::Error, "Failed to initialize input, window must be initialized first");
            return;
        }

        s_Enabled = true;
        Logger::Log(LogLevel::Info, "Initialized input");
    }

    void Input::Shutdown()
    {
        if (s_Enabled)
        {
            s_Enabled = false;
            s_WindowHandle = nullptr;
            Logger::Log(LogLevel::Info, "Input shutdown");
        }
        else
        {
            Logger::LogWarn("Can't shutdown input, it hasn't been initialized first");
        }
    }

    bool Input::IsKeyPressed(KeyCode keycode)
    {
        if (s_Enabled)
        {
            if (glfwGetKey(s_WindowHandle, (int32_t)keycode) == GLFW_PRESS)
            {
                return true;
            }
        }
        return false;
    }

    bool Input::IsKeyHeld(KeyCode keycode)
    {
        if (s_Enabled)
        {
            if (glfwGetKey(s_WindowHandle, (int32_t)keycode) == GLFW_PRESS || GLFW_REPEAT)
            {
                return true;
            }
        }
        return false;
    }
}