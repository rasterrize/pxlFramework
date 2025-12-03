#include "Input.h"

#include "Events/MouseEvents.h"

namespace pxl
{
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
        s_InputSystem = window->GetInputSystem();
        s_Enabled = true;

        PXL_LOG_INFO(LogArea::Input, "Input initialized");
    }

    void Input::Shutdown()
    {
        if (!s_Enabled)
            return;

        s_Enabled = false;
        s_WindowHandle = nullptr;
        PXL_LOG_INFO(LogArea::Input, "Input shutdown");
    }

    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(s_Enabled);

        if (!s_InputSystem->GetCurrentState().KeyStates.contains(keyCode))
            return false;

        return s_InputSystem->GetCurrentState().KeyStates.at(keyCode) == GLFW_PRESS;
    }

    bool Input::IsKeyHeld(KeyCode keyCode)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(s_Enabled);

        return glfwGetKey(s_WindowHandle, static_cast<int>(keyCode)) == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPressed(MouseCode buttonCode)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(s_Enabled);

        if (!s_InputSystem->GetCurrentState().MBStates.contains(buttonCode) || !s_InputSystem->GetPreviousState().MBStates.contains(buttonCode))
            return false;

        return s_InputSystem->GetCurrentState().MBStates.at(buttonCode) == GLFW_PRESS && s_InputSystem->GetPreviousState().MBStates.at(buttonCode) != GLFW_PRESS;
    }

    bool Input::IsMouseButtonHeld(MouseCode buttonCode)
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(s_Enabled);

        if (!s_InputSystem->GetCurrentState().MBStates.contains(buttonCode))
            return false;

        return s_InputSystem->GetCurrentState().MBStates.at(buttonCode) == GLFW_PRESS;
    }

    bool Input::IsMouseScrolledUp()
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(s_Enabled);

        if (s_InputSystem->GetCurrentState().VerticalScrollOffset > 0.0)
            return true;
        
        return false;
    }

    bool Input::IsMouseScrolledDown()
    {
        PXL_PROFILE_SCOPE;

        PXL_ASSERT(s_Enabled);

        if (s_InputSystem->GetCurrentState().VerticalScrollOffset < 0.0)
            return true;

        return false;
    }

    void Input::SetCursorPosition(double x, double y)
    {
        PXL_ASSERT(s_Enabled);

        glfwSetCursorPos(s_WindowHandle, x, y);
    }

    glm::vec2 Input::GetCursorDelta()
    {
        auto currentPos = s_InputSystem->GetCurrentState().CursorPosition;
        auto previousPos = s_InputSystem->GetPreviousState().CursorPosition;

        return currentPos - previousPos;
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
            case CursorMode::Captured:
                glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                PXL_LOG_INFO(LogArea::Input, "Cursor mode set to Captured");
                return;
        }
    }

    bool Input::GetRawInput()
    {
        return glfwGetInputMode(s_WindowHandle, GLFW_RAW_MOUSE_MOTION);
    }

    void Input::SetRawInput(bool value)
    {
        PXL_ASSERT(s_Enabled);

        if (!s_RawInputSupported)
        {
            PXL_LOG_WARN(LogArea::Input, "Can't set Raw Input because it isn't supported");
            return;
        }

        if (value)
            PXL_LOG_INFO(LogArea::Input, "Enabled Raw Input")
        else
            PXL_LOG_INFO(LogArea::Input, "Disabled Raw Input");

        glfwSetInputMode(s_WindowHandle, GLFW_RAW_MOUSE_MOTION, value);
    }

    void Input::SetCursorVisibility(bool visible)
    {
        PXL_ASSERT(s_Enabled);

        glfwSetInputMode(s_WindowHandle, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }

    void Input::SetCursor(StandardCursor standardCursor)
    {
        int32_t cursorType = 0;

        switch (standardCursor)
        {
            case StandardCursor::Arrow:     cursorType = GLFW_ARROW_CURSOR; break;
            case StandardCursor::IBeam:     cursorType = GLFW_IBEAM_CURSOR; break;
            case StandardCursor::Crosshair: cursorType = GLFW_CROSSHAIR_CURSOR; break;
            case StandardCursor::Hand:      cursorType = GLFW_HAND_CURSOR; break;
            case StandardCursor::HResize:   cursorType = GLFW_HRESIZE_CURSOR; break;
            case StandardCursor::VResize:   cursorType = GLFW_VRESIZE_CURSOR; break;
        }

        glfwSetCursor(s_WindowHandle, glfwCreateStandardCursor(cursorType));
    }

    void Input::SetCursor(Cursor customCursor)
    {
        glfwSetCursor(s_WindowHandle, customCursor.GetNativeCursor());
    }
}