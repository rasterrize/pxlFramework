#include "Input.h"

namespace pxl
{
    GLFWwindow* Input::s_WindowHandle = nullptr;
    bool Input::s_Enabled = false;
    std::unordered_map<int, int> Input::s_CurrentKeyStates;
    std::unordered_map<int, int> Input::s_PreviousKeyStates;
    std::unordered_map<int, int> Input::s_CurrentMBStates;
    std::unordered_map<int, int> Input::s_PreviousMBStates;
    double Input::s_VerticalScrollOffset = 0.0f;
    double Input::s_HorizontalScrollOffset = 0.0f;
    glm::vec2 Input::s_CursorPosition = glm::vec2(0.0f);

    void Input::Init(const std::shared_ptr<Window>& window)
    {
        PXL_ASSERT_MSG(window, "Can't initialize input since window was invalid");
        
        if (s_Enabled)
        {
            PXL_LOG_ERROR(LogArea::Input, "Failed to initalize input, it's already initialized");
            return;
        }

        s_WindowHandle = window->GetNativeWindow();
        s_Enabled = true;

        PXL_LOG_INFO(LogArea::Input, "Input initialized");
    }

    void Input::Shutdown()
    {
        if (s_Enabled)
        {
            s_Enabled = false;
            s_WindowHandle = nullptr;
            PXL_LOG_INFO(LogArea::Input, "Input shutdown");
        }
    }

    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return false;

        bool keyPressed = s_CurrentKeyStates[keyCode] == GLFW_PRESS && s_PreviousKeyStates[keyCode] != GLFW_PRESS ? true : false;

        s_PreviousKeyStates[keyCode] = s_CurrentKeyStates[keyCode];
        
        return keyPressed;
    }

    bool Input::IsKeyHeld(KeyCode keyCode)
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return false;

        if (glfwGetKey(s_WindowHandle, keyCode) == GLFW_PRESS)
            return true;

        return false;
    }

    bool Input::IsMouseButtonPressed(MouseCode buttonCode)
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return false;

        bool buttonPressed = s_CurrentMBStates[buttonCode] == GLFW_PRESS && s_PreviousMBStates[buttonCode] != GLFW_PRESS ? true : false;
        
        s_PreviousMBStates[buttonCode] = s_CurrentMBStates[buttonCode];

        return buttonPressed;
    }

    bool Input::IsMouseButtonHeld(MouseCode buttonCode)
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return false;

        if (glfwGetMouseButton(s_WindowHandle, buttonCode) == GLFW_PRESS)
            return true;

        return false;
    }

    bool Input::IsMouseScrolledUp()
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return false;

        bool scrolledUp = false;

        if (s_VerticalScrollOffset == 1.0f)
        {
            scrolledUp = true;
            s_VerticalScrollOffset = 0.0f;
        }

        return scrolledUp;
    }

    bool Input::IsMouseScrolledDown()
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled)
            return false;

        bool scrolledUp = false;

        if (s_VerticalScrollOffset == -1.0f)
        {
            scrolledUp = true;
            s_VerticalScrollOffset = 0.0f;
        }

        return scrolledUp;
    }

    void Input::SetCursorPosition(uint32_t x, uint32_t y)
    {
        if (!s_Enabled)
            return;
        
        // TODO: check if x and y values are outside the bounds of the window

        glfwSetCursorPos(s_WindowHandle, x, y);
        s_CursorPosition.x = static_cast<float>(x);
        s_CursorPosition.y = static_cast<float>(y);
    }

    void Input::SetCursorMode(CursorMode cursorMode)
    {
        if (!s_Enabled)
            return;
        
        switch (cursorMode)
        {
            case CursorMode::Normal:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case CursorMode::Hidden:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            case CursorMode::Disabled:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
        }
    }

    void Input::SetRawInput(bool value)
    {
        if (value)
        {
            if (glfwRawMouseMotionSupported()) // TODO: Ensure this function is only called once
            {
                glfwSetInputMode(s_WindowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                PXL_LOG_INFO(LogArea::Input, "Enabled Raw Input");
            }
            else
            {
                PXL_LOG_WARN(LogArea::Input, "Failed to enabled Raw Input because it isn't supported");
            }
        }
        else
            glfwSetInputMode(s_WindowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }

    void Input::SetCursorVisibility(bool visible)
    {
        if (!s_Enabled)
            return;

        if (visible)
            glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
        else
            glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
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

    void Input::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        s_VerticalScrollOffset = yoffset;
    }
}