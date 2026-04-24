#pragma once

#include "Core/Window.h"
#include "Event.h"

namespace pxl
{
    class WindowEvent : public Event
    {
    public:
        WindowEvent(const std::shared_ptr<Window>& window)
            : m_Window(window)
        {
        }

        // Returns the window this event was sourced from. Useful for detecting events from multiple windows, can be ignored otherwise.
        std::shared_ptr<Window> GetWindow() const { return m_Window.lock(); }

    private:
        std::weak_ptr<Window> m_Window;
    };

    class WindowResizeEvent : public WindowEvent
    {
    public:
        WindowResizeEvent(const std::shared_ptr<Window>& window, Size2D size)
            : WindowEvent(window), m_Size(size)
        {
        }

        static EventType GetStaticType() { return EventType::WindowResize; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Size = {}", m_Size.ToString()); }

        Size2D GetSize() const { return m_Size; }

    private:
        Size2D m_Size = {};
    };

    class WindowFBResizeEvent : public WindowResizeEvent
    {
    public:
        WindowFBResizeEvent(const std::shared_ptr<Window>& window, Size2D size)
            : WindowResizeEvent(window, size)
        {
        }

        static EventType GetStaticType() { return EventType::WindowFBResize; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
    };

    class WindowRepositionEvent : public WindowEvent
    {
    public:
        WindowRepositionEvent(const std::shared_ptr<Window>& window, const glm::ivec2& pos)
            : WindowEvent(window), m_Position(pos)
        {
        }

        static EventType GetStaticType() { return EventType::WindowReposition; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Position = {}, {}", m_Position.x, m_Position.y); }

        glm::ivec2 GetPosition() const { return m_Position; }

    private:
        glm::ivec2 m_Position = {};
    };

    class WindowModeChangeEvent : public WindowEvent
    {
    public:
        WindowModeChangeEvent(const std::shared_ptr<Window>& window, WindowMode mode)
            : WindowEvent(window), m_WindowMode(mode)
        {
        }

        static EventType GetStaticType() { return EventType::WindowModeChange; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Mode = {}", Utils::ToString(m_WindowMode)); }

        WindowMode GetMode() const { return m_WindowMode; }

    private:
        WindowMode m_WindowMode = {};
    };

    class WindowMinimizeEvent : public WindowEvent
    {
    public:
        WindowMinimizeEvent(const std::shared_ptr<Window>& window, bool minimized)
            : WindowEvent(window), m_Minimized(minimized)
        {
        }

        static EventType GetStaticType() { return EventType::WindowMinimize; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Minimized = {}", m_Minimized); }

        bool WasMinimized() const { return m_Minimized; }

    private:
        bool m_Minimized = false;
    };

    class WindowPathDropEvent : public WindowEvent
    {
    public:
        WindowPathDropEvent(const std::shared_ptr<Window>& window, const std::vector<std::string>& paths)
            : WindowEvent(window), m_Paths(paths)
        {
        }

        static EventType GetStaticType() { return EventType::WindowPathDrop; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override
        {
            std::string concatenatedPaths;
            for (int i = 0; i < m_Paths.size(); i++)
                concatenatedPaths.append(std::format("{}: {}\n", i, m_Paths.at(i)));

            return std::format("{} Paths: \n{}", m_Paths.size(), concatenatedPaths);
        }

        std::vector<std::string> GetPaths() const { return m_Paths; }

    private:
        std::vector<std::string> m_Paths;
    };

    class WindowFocusEvent : public WindowEvent
    {
    public:
        WindowFocusEvent(const std::shared_ptr<Window>& window, bool focused)
            : WindowEvent(window), m_Focused(focused)
        {
        }

        static EventType GetStaticType() { return EventType::WindowFocus; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Focused = {}", m_Focused); }

        bool IsFocused() const { return m_Focused; }

    private:
        bool m_Focused = false;
    };

    class WindowCloseEvent : public WindowEvent
    {
    public:
        WindowCloseEvent(const std::shared_ptr<Window>& window)
            : WindowEvent(window)
        {
        }

        static EventType GetStaticType() { return EventType::WindowClose; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return "Closing"; }
    };

    class WindowCursorEnterEvent : public WindowEvent
    {
    public:
        WindowCursorEnterEvent(const std::shared_ptr<Window>& window, bool entered)
            : WindowEvent(window), m_Entered(entered)
        {
        }

        static EventType GetStaticType() { return EventType::WindowCursorEnter; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Entered = {}", m_Entered); }

        bool WasEntered() const { return m_Entered; }
        bool WasLeft() const { return !m_Entered; }

    private:
        bool m_Entered = false;
    };
}