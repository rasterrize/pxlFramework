#pragma once

namespace pxl
{
    enum class EventType
    {
        Unknown = 0,

        // Keyboard events
        KeyDown,
        KeyUp,
        KeyRepeat,

        // Mouse events
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        MouseScroll,

        // Window events
        WindowResize,
        WindowReposition,
        WindowMinimize,
        WindowModeChange,
        WindowPathDrop,
        WindowFBResize,
        WindowFocus,
        WindowClose,
        WindowCursorEnter,

        // Gamepad events
        GamepadAxisChange,
        GamepadButtonDown,
        GamepadButtonUp,
        GamepadStatusChange,
    };

    namespace Utils
    {
        inline std::string ToString(EventType type)
        {
            switch (type)
            {
                case EventType::Unknown:   return "Unknown";
                case EventType::KeyDown:   return "KeyDown";
                case EventType::KeyUp:     return "KeyUp";
                case EventType::KeyRepeat: return "KeyRepeat";

                case EventType::MouseMove:       return "MouseMove";
                case EventType::MouseButtonDown: return "MouseButtonDown";
                case EventType::MouseButtonUp:   return "MouseButtonUp";
                case EventType::MouseScroll:     return "MouseScroll";

                case EventType::WindowResize:     return "WindowResize";
                case EventType::WindowReposition: return "WindowReposition";
                case EventType::WindowMinimize:   return "WindowMinimize";
                case EventType::WindowModeChange: return "WindowModeChange";
                case EventType::WindowPathDrop:   return "WindowPathDrop";
                case EventType::WindowFBResize:   return "WindowFBResize";
                case EventType::WindowFocus:      return "WindowFocus";
                case EventType::WindowClose:      return "WindowClose";

                case EventType::GamepadAxisChange:   return "GamepadAxisChange";
                case EventType::GamepadButtonDown:   return "GamepadButtonDown";
                case EventType::GamepadButtonUp:     return "GamepadButtonUp";
                case EventType::GamepadStatusChange: return "GamepadConnection";
                default:                             return "Unknown";
            }
        }
    }
}