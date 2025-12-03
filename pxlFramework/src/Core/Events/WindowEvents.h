#pragma once

#include "Core/Window.h"
#include "Event.h"
#include "Utils/EnumStringHelper.h"

namespace pxl
{
    class WindowEvent : public Event
    {
    public:
        WindowEvent(EventType type, const std::shared_ptr<Window>& window)
            : Event(type), m_Window(window)
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
        WindowResizeEvent(Size2D size, const std::shared_ptr<Window>& window)
            : WindowEvent(EventType::WindowResize, window), m_Size(size)
        {
        }

        Size2D GetSize() const { return m_Size; }

        static EventType GetStaticType() { return EventType::WindowResize; }

    protected:
        virtual std::string DataToString() const override { return std::format("Size = {}, {}", m_Size.Width, m_Size.Height); }

    private:
        Size2D m_Size;
    };

    class WindowRepositionEvent : public WindowEvent
    {
    public:
        WindowRepositionEvent(const glm::ivec2& pos, const std::shared_ptr<Window>& window)
            : WindowEvent(EventType::WindowReposition, window), m_Position(pos)
        {
        }

        glm::ivec2 GetPosition() const { return m_Position; }

        static EventType GetStaticType() { return EventType::WindowReposition; }

    protected:
        virtual std::string DataToString() const override { return std::format("Position = {}, {}", m_Position.x, m_Position.y); }

    private:
        glm::ivec2 m_Position;
    };

    class WindowModeChangeEvent : public WindowEvent
    {
    public:
        WindowModeChangeEvent(WindowMode mode, const std::shared_ptr<Window>& window)
            : WindowEvent(EventType::WindowModeChange, window), m_WindowMode(mode)
        {
        }

        WindowMode GetMode() const { return m_WindowMode; }

        static EventType GetStaticType() { return EventType::WindowModeChange; }

    protected:
        virtual std::string DataToString() const override { return std::format("Mode = {}", EnumStringHelper::ToString(m_WindowMode)); }

    private:
        WindowMode m_WindowMode;
    };

    class WindowMinimizeEvent : public WindowEvent
    {
    public:
        WindowMinimizeEvent(bool minimized, const std::shared_ptr<Window>& window)
            : WindowEvent(EventType::WindowMinimize, window), m_Minimized(minimized)
        {
        }

        bool GetMinimized() const { return m_Minimized; }

        static EventType GetStaticType() { return EventType::WindowMinimize; }

    protected:
        virtual std::string DataToString() const override { return std::format("Minimized = {}", m_Minimized); }

    private:
        bool m_Minimized;
    };

    class WindowPathDropEvent : public WindowEvent
    {
    public:
        WindowPathDropEvent(const std::vector<std::string>& paths, const std::shared_ptr<Window>& window)
            : WindowEvent(EventType::WindowPathDrop, window), m_Paths(paths)
        {
        }

        std::vector<std::string> GetPaths() const { return m_Paths; }

        static EventType GetStaticType() { return EventType::WindowPathDrop; }

    protected:
        virtual std::string DataToString() const override
        {
            std::string concatenatedPaths;
            for (int i = 0; i < m_Paths.size(); i++)
                concatenatedPaths.append(std::format("{}: {}\n", i, m_Paths.at(i)));

            return std::format("{} Paths: \n{}", m_Paths.size(), concatenatedPaths);
        }

    private:
        std::vector<std::string> m_Paths;
    };
}