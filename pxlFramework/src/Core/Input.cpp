#include "Input.h"
#include "Window.h"
#include "Application.h"

namespace pxl
{
    GLFWwindow* Input::s_WindowHandle;
    bool Input::s_Enabled = false;
    std::unordered_map<int, int> Input::s_CurrentKeyStates;
    std::unordered_map<int, int> Input::s_PreviousKeyStates;
    std::unordered_map<int, int> Input::s_CurrentMBStates;
    std::unordered_map<int, int> Input::s_PreviousMBStates;
    glm::vec2 Input::s_CursorPosition = glm::vec2(1.0f);

    void Input::Init(const std::shared_ptr<Window> window)
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

        s_WindowHandle = static_cast<GLFWwindow*>(window->GetNativeWindow());
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

    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        if (!s_Enabled)
            return false;

        bool keyPressed = false;
        
        if (s_CurrentKeyStates[keyCode] == GLFW_PRESS && s_PreviousKeyStates[keyCode] != GLFW_PRESS)
        {
            keyPressed = true;
        }
        else
        {
            keyPressed = false;
        }

        s_PreviousKeyStates[keyCode] = s_CurrentKeyStates[keyCode];
        return keyPressed;
    }

    bool Input::IsKeyHeld(KeyCode keyCode)
    {
        if (!s_Enabled)
            return false;

        if (glfwGetKey(s_WindowHandle, keyCode) == GLFW_PRESS)
            return true;

        return false;
    }

    bool Input::IsMouseButtonPressed(ButtonCode buttonCode)
    {
        if (!s_Enabled)
            return false;

        bool buttonPressed = false;
        
        if (s_CurrentMBStates[buttonCode] == GLFW_PRESS && s_PreviousMBStates[buttonCode] != GLFW_PRESS)
        {
            buttonPressed = true;
        }
        else
        {
            buttonPressed = false;
        }

        s_PreviousMBStates[buttonCode] = s_CurrentMBStates[buttonCode];

        return buttonPressed;
    }

    bool Input::IsMouseButtonHeld(ButtonCode buttonCode)
    {
        if (!s_Enabled)
            return false;

        if (glfwGetMouseButton(s_WindowHandle, buttonCode) == GLFW_PRESS)
            return true;

        return false;
    }

    void Input::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        s_CurrentKeyStates[key] = action;
    }

    void Input::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        s_CurrentMBStates[button] = action;
    }

    void Input::GLFWCursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        s_CursorPosition = glm::vec2((float)xpos, (float)ypos);
    }
}