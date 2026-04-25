#pragma once

#include <GLFW/glfw3.h>

#include "Cursor.h"
#include "Events/Event.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

namespace pxl
{
    struct InputState
    {
        std::unordered_map<KeyCode, int> KeyStates;
        std::unordered_map<MouseCode, int> MBStates;

        double VerticalScrollOffset = 0.0;
        double HorizontalScrollOffset = 0.0;

        glm::dvec2 CursorPosition = {};
    };

    class InputSystem
    {
    public:
        InputSystem(GLFWwindow* window, const std::function<void(std::unique_ptr<Event>)>& eventCallback);

        void ResetCurrentState();

        const InputState& GetCurrentState() const { return m_CurrentInputState; }
        const InputState& GetPreviousState() const { return m_PreviousInputState; }

        void SetRawInput(bool enable);

    private:
        GLFWwindow* m_Window = nullptr;

        InputState m_CurrentInputState = {};
        InputState m_PreviousInputState = {};

        std::function<void(std::unique_ptr<Event>)> m_EventCallback;

        bool m_RawInputSupported = false;
    };

    namespace Utils
    {
        inline int ToGLFWCursorMode(CursorMode mode)
        {
            switch (mode)
            {
                case CursorMode::Normal:   return GLFW_CURSOR_NORMAL;
                case CursorMode::Hidden:   return GLFW_CURSOR_HIDDEN;
                case CursorMode::Disabled: return GLFW_CURSOR_DISABLED;
                case CursorMode::Captured: return GLFW_CURSOR_CAPTURED;
                default:                   return GLFW_CURSOR_NORMAL;
            }
        }

        inline int ToGLFWStandardCursor(StandardCursor cursor)
        {
            switch (cursor)
            {
                case StandardCursor::Arrow:     return GLFW_ARROW_CURSOR;
                case StandardCursor::IBeam:     return GLFW_IBEAM_CURSOR;
                case StandardCursor::Crosshair: return GLFW_CROSSHAIR_CURSOR;
                case StandardCursor::Hand:      return GLFW_HAND_CURSOR;
                case StandardCursor::HResize:   return GLFW_HRESIZE_CURSOR;
                case StandardCursor::VResize:   return GLFW_VRESIZE_CURSOR;
                default:                        return GLFW_ARROW_CURSOR;
            }
        }
    }
}