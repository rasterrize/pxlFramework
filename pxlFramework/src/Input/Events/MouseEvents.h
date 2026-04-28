#pragma once

#include <glm/vec2.hpp>

#include "Input/MouseCodes.h"
#include "InputEvent.h"

namespace pxl
{
    // ------------------------
    // MouseMoveEvent
    // ------------------------

    class MouseMoveEvent : public InputEvent
    {
    public:
        MouseMoveEvent(const std::shared_ptr<InputSystem>& system, const glm::dvec2& position)
            : InputEvent(system), m_Position(position), m_Delta(position - system->GetPreviousState().CursorPosition)
        {
        }

        static EventType GetStaticType() { return EventType::MouseMove; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Position = {}, {}", m_Position.x, m_Position.y); }

        glm::dvec2 GetPosition() const { return m_Position; }

    private:
        glm::dvec2 m_Position = {};
        glm::dvec2 m_Delta = {};
    };

    // ------------------------
    // MouseButtonEvent
    // ------------------------

    class MouseButtonEvent : public InputEvent
    {
    public:
        MouseButtonEvent(const std::shared_ptr<InputSystem>& system, MouseCode button, int mods)
            : InputEvent(system), m_Button(button), m_Modifiers(mods)
        {
        }

        virtual std::string DataToString() const override { return std::format("Button = {}", Utils::ToString(m_Button)); }

        MouseCode GetButton() const { return m_Button; }

        bool IsButton(MouseCode button) const { return m_Button == button; }
        bool IsMods(int mods) const { return (m_Modifiers & mods) == mods; }
        bool IsModsAndButton(int mods, MouseCode button) const { return IsMods(mods) && IsButton(button); }

    private:
        MouseCode m_Button = {};
        int m_Modifiers = 0;
    };

    // ------------------------
    // MouseButtonDownEvent
    // ------------------------

    class MouseButtonDownEvent : public MouseButtonEvent
    {
    public:
        MouseButtonDownEvent(const std::shared_ptr<InputSystem>& system, MouseCode button, int mods)
            : MouseButtonEvent(system, button, mods)
        {
        }

        static EventType GetStaticType() { return EventType::MouseButtonDown; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
    };

    // ------------------------
    // MouseButtonUpEvent
    // ------------------------

    class MouseButtonUpEvent : public MouseButtonEvent
    {
    public:
        MouseButtonUpEvent(const std::shared_ptr<InputSystem>& system, MouseCode button, int mods)
            : MouseButtonEvent(system, button, mods)
        {
        }

        static EventType GetStaticType() { return EventType::MouseButtonUp; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
    };

    // ------------------------
    // MouseScrollEvent
    // ------------------------

    class MouseScrollEvent : public InputEvent
    {
    public:
        MouseScrollEvent(const std::shared_ptr<InputSystem>& system, double vOffset, double hOffset)
            : InputEvent(system), m_VOffset(vOffset), m_HOffset(hOffset)
        {
        }

        static EventType GetStaticType() { return EventType::MouseScroll; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("VOffset = {}, HOffset = {}", m_VOffset, m_HOffset); }

        double GetVerticalOffset() const { return m_VOffset; }
        double GetHorizontalOffset() const { return m_HOffset; }

    private:
        double m_VOffset = 0.0;
        double m_HOffset = 0.0;
    };
}