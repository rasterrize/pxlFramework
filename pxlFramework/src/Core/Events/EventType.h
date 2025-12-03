#pragma once

namespace pxl
{
    enum class EventType
    {
        Unknown = 0,

        // Keyboard events
        KeyDown = 20,
        KeyUp,

        // Mouse events
        MouseMove = 40,
        MouseButtonDown,
        MouseButtonUp,
        MouseScroll,

        // Window events
        WindowResize = 60,
        WindowReposition,
        WindowMinimize,
        WindowModeChange,
        WindowPathDrop,
    };

    namespace Utils
    {
        inline std::string ToString(EventType type)
        {
            switch (type)
            {
                case EventType::Unknown:          return "Unknown";
                case EventType::KeyDown:          return "KeyDown";
                case EventType::KeyUp:            return "KeyUp";
                case EventType::MouseMove:        return "MouseMove";
                case EventType::MouseButtonDown:  return "MouseButtonDown";
                case EventType::MouseButtonUp:    return "MouseButtonUp";
                case EventType::MouseScroll:      return "MouseScroll";
                case EventType::WindowResize:     return "WindowResize";
                case EventType::WindowReposition: return "WindowReposition";
                case EventType::WindowMinimize:   return "WindowMinimize";
                case EventType::WindowModeChange: return "WindowModeChange";
                case EventType::WindowPathDrop:   return "WindowPathDrop";
            }

            return "Undefined";
        }
    }
}