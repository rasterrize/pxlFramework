#pragma once

#include <glm/glm.hpp>

#include "Core/MouseCodes.h"
#include "InputEvent.h"

namespace pxl
{
    // ------------------------
    // MouseMoveEvent
    // ------------------------

    class MouseMoveEvent : public InputEvent
    {
    public:
        MouseMoveEvent(const glm::dvec2& position, const std::shared_ptr<InputSystem>& system)
            : InputEvent(EventType::MouseMove, system), m_Position(position)
        {
        }

        glm::dvec2 GetPosition() const { return m_Position; }

        static EventType GetStaticType() { return EventType::MouseMove; }

    protected:
        virtual std::string DataToString() const override { return std::format("Position = {}, {}", m_Position.x, m_Position.y); }

    private:
        glm::dvec2 m_Position;
    };

    // ------------------------
    // MouseButtonDownEvent
    // ------------------------

    class MouseButtonDownEvent : public InputEvent
    {
    public:
        MouseButtonDownEvent(MouseCode button, const std::shared_ptr<InputSystem>& system)
            : InputEvent(EventType::MouseButtonDown, system), m_Button(button)
        {
        }

        MouseCode GetButton() const { return m_Button; }

        static EventType GetStaticType() { return EventType::MouseButtonDown; }

    protected:
        virtual std::string DataToString() const override { return std::format("Button = {}", Utils::ToString(m_Button)); }

    private:
        MouseCode m_Button;
    };

    // ------------------------
    // MouseButtonUpEvent
    // ------------------------

    class MouseButtonUpEvent : public InputEvent
    {
    public:
        MouseButtonUpEvent(MouseCode button, const std::shared_ptr<InputSystem>& system)
            : InputEvent(EventType::MouseButtonUp, system), m_Button(button)
        {
        }

        MouseCode GetButton() const { return m_Button; }

        static EventType GetStaticType() { return EventType::MouseButtonUp; }

    protected:
        virtual std::string DataToString() const override { return std::format("Button = {}", Utils::ToString(m_Button)); }

    private:
        MouseCode m_Button;
    };

    // ------------------------
    // MouseScrollEvent
    // ------------------------

    class MouseScrollEvent : public InputEvent
    {
    public:
        MouseScrollEvent(double vOffset, double hOffset, const std::shared_ptr<InputSystem>& system)
            : InputEvent(EventType::MouseScroll, system), m_VOffset(vOffset), m_HOffset(hOffset)
        {
        }

        double GetVerticalOffset() const { return m_VOffset; }
        double GetHorizontalOffset() const { return m_HOffset; }

        static EventType GetStaticType() { return EventType::MouseScroll; }

    protected:
        virtual std::string DataToString() const override { return std::format("VOffset = {}, HOffset = {}", m_VOffset, m_HOffset); }

    private:
        double m_VOffset;
        double m_HOffset;
    };
}