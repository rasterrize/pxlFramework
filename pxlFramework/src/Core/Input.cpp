#include "Input.h"

namespace pxl
{
    bool Input::s_Enabled = false;

    GLFWwindow* Input::s_WindowHandle = nullptr;

    std::unordered_map<int, int> Input::s_CurrentKeyStates;
    std::unordered_map<int, int> Input::s_PreviousKeyStates;

    std::unordered_map<int, int> Input::s_CurrentMBStates;
    std::unordered_map<int, int> Input::s_PreviousMBStates;

    double Input::s_VerticalScrollOffset = 0.0f;
    double Input::s_HorizontalScrollOffset = 0.0f;

    glm::dvec2 Input::s_CursorPosition = glm::dvec2(0.0f);

    bool Input::s_RawInputSupported = false;

    void Input::Init(const std::shared_ptr<Window>& window)
    {
        PXL_ASSERT_MSG(window, "Can't initialize input since window was invalid");
        
        if (s_Enabled)
        {
            PXL_LOG_WARN(LogArea::Input, "Failed to initalize input, it's already initialized");
            return;
        }

        // Check for raw input support
        s_RawInputSupported = glfwRawMouseMotionSupported();

        s_WindowHandle = window->GetNativeWindow();
        s_Enabled = true;

        PXL_LOG_INFO(LogArea::Input, "Input initialized");
    }

    void Input::Shutdown()
    {
        s_Enabled = false;
        s_WindowHandle = nullptr;
        PXL_LOG_INFO(LogArea::Input, "Input shutdown");
    }

    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        PXL_PROFILE_SCOPE;
        
        PXL_ASSERT(s_Enabled);

        bool keyPressed = s_CurrentKeyStates[keyCode] == GLFW_PRESS && s_PreviousKeyStates[keyCode] != GLFW_PRESS ? true : false;

        s_PreviousKeyStates[keyCode] = s_CurrentKeyStates[keyCode];
        
        return keyPressed;
    }

    bool Input::IsKeyHeld(KeyCode keyCode)
    {
        PXL_PROFILE_SCOPE;
        
        PXL_ASSERT(s_Enabled);

        return glfwGetKey(s_WindowHandle, keyCode) == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPressed(MouseCode buttonCode)
    {
        PXL_PROFILE_SCOPE;
        
        PXL_ASSERT(s_Enabled);

        bool buttonPressed = s_CurrentMBStates[buttonCode] == GLFW_PRESS && s_PreviousMBStates[buttonCode] != GLFW_PRESS ? true : false;
        
        s_PreviousMBStates[buttonCode] = s_CurrentMBStates[buttonCode];

        return buttonPressed;
    }

    bool Input::IsMouseButtonHeld(MouseCode buttonCode)
    {
        PXL_PROFILE_SCOPE;
        
        PXL_ASSERT(s_Enabled);

        return glfwGetMouseButton(s_WindowHandle, buttonCode) == GLFW_PRESS;
    }

    bool Input::IsMouseScrolledUp()
    {
        PXL_PROFILE_SCOPE;
        
        PXL_ASSERT(s_Enabled);

        if (s_VerticalScrollOffset == 1.0f)
        {
            s_VerticalScrollOffset = 0.0f;
            return true;
        }

        return false;
    }

    bool Input::IsMouseScrolledDown()
    {
        PXL_PROFILE_SCOPE;
        
        PXL_ASSERT(s_Enabled);

        if (s_VerticalScrollOffset == -1.0f)
        {
            s_VerticalScrollOffset = 0.0f;
            return true;
        }

        return false;
    }

    void Input::SetCursorPosition(uint32_t x, uint32_t y)
    {
        PXL_ASSERT(s_Enabled);

        // TODO: check if x and y values are outside the bounds of the window

        glfwSetCursorPos(s_WindowHandle, static_cast<double>(x), static_cast<double>(y));
        s_CursorPosition.x = static_cast<double>(x);
        s_CursorPosition.y = static_cast<double>(y);
    }

    void Input::SetCursorMode(CursorMode cursorMode)
    {
        PXL_ASSERT(s_Enabled);

        switch (cursorMode)
        {
            case CursorMode::Normal:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                PXL_LOG_INFO(LogArea::Input, "Cursor mode set to Normal");
                return;
            case CursorMode::Hidden:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                PXL_LOG_INFO(LogArea::Input, "Cursor mode set to Hidden");
                return;
            case CursorMode::Disabled:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                PXL_LOG_INFO(LogArea::Input, "Cursor mode set to Disabled");
                return;
        }
    }

    void Input::SetRawInput(bool value)
    {
        PXL_ASSERT(s_Enabled);
        
        if (!s_RawInputSupported)
        {
            PXL_LOG_WARN(LogArea::Input, "Can't set Raw Input because it isn't supported");
            return;
        }

        if (value && s_RawInputSupported)
        {
            glfwSetInputMode(s_WindowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            PXL_LOG_INFO(LogArea::Input, "Enabled Raw Input");
        }
        else
        {
            PXL_LOG_INFO(LogArea::Input, "Disabled Raw Input");
        }
    }

    void Input::SetCursorVisibility(bool visible)
    {
        PXL_ASSERT(s_Enabled);

        visible ? glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL)
                : glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
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
        s_CursorPosition = { xpos, ypos };
    }

    void Input::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        s_VerticalScrollOffset = yoffset;
    }
}