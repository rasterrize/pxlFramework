#include "Input.h"
#include "Window.h"
#include "Application.h"

namespace pxl
{
    GLFWwindow* Input::s_WindowHandle;
    bool Input::s_Enabled = false;
    std::unordered_map<int, int> Input::s_CurrentKeyStates;
    std::unordered_map<int, int> Input::s_PreviousKeyStates;

    void Input::Init()
    {
        if (s_Enabled)
        {
            Logger::LogWarn("Failed to initalize input, it's already initialized");
            return;
        }
        
        if (!Application::Get().IsRunning())
        {
            Logger::LogError("Can't initialize input, no application exists");
            return;
        }

        s_WindowHandle = Window::GetNativeWindow();
        if (!s_WindowHandle)
        {
            Logger::Log(LogLevel::Error, "Failed to initialize input, window must be initialized first");
            return;
        }

        s_Enabled = true;
        Logger::Log(LogLevel::Info, "Input initialized");
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
        if (!s_Enabled)
            return false;

        bool keyPressed = false;
        
        if (s_CurrentKeyStates[keycode] == GLFW_PRESS && s_PreviousKeyStates[keycode] != GLFW_PRESS)
        {
            keyPressed = true;
        }
        else
        {
            keyPressed = false;
        }

        s_PreviousKeyStates[keycode] = s_CurrentKeyStates[keycode];
        return keyPressed;
    }

    bool Input::IsKeyHeld(KeyCode keycode)
    {
        if (s_Enabled)
        {
            if (glfwGetKey(s_WindowHandle, keycode) == GLFW_PRESS)
            {
                return true;
            }
        }
        return false;
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        s_CurrentKeyStates[key] = action;
    }
}