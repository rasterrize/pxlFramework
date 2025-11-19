#pragma once

#include "Core/Gamepad.h"
#include "Event.h"

namespace pxl
{
    class GamepadEvent : public Event
    {
    protected:
        GamepadEvent(EventType type, int gamepadJID)
            : Event(type), m_JID(gamepadJID)
        {
        }

        int GetGamepadJID() const { return m_JID; }

    protected:
        int m_JID;
    };

    class GamepadAxisChangeEvent : public GamepadEvent
    {
    public:
        GamepadAxisChangeEvent(int gamepadJID, GamepadAxis axis, float value)
            : GamepadEvent(EventType::GamepadAxisChange, gamepadJID), m_Axis(axis), m_Value(value)
        {
        }

        GamepadAxis GetAxis() const { return m_Axis; }
        float GetAxisValue() const { return m_Value; }

        bool IsAxis(GamepadAxis axis) const { return axis == m_Axis; }
        bool IsThumbstick() const { return static_cast<int32_t>(m_Axis) < static_cast<int32_t>(GamepadAxis::LeftTrigger); }

        static EventType GetStaticType() { return EventType::GamepadAxisChange; }

    protected:
        virtual std::string DataToString() const override { return std::format("Controller = {}, Axis = {}, Value = {}", m_JID, Utils::ToString(m_Axis), m_Value); }

    private:
        GamepadAxis m_Axis;
        float m_Value;
    };
    
    class GamepadButtonEvent : public GamepadEvent
    {
    protected:
        GamepadButtonEvent(EventType type, int gamepadJID, GamepadButton button)
            : GamepadEvent(type, gamepadJID), m_Button(button)
        {
        }

        GamepadButton GetButton() const { return m_Button; }

        bool IsButton(GamepadButton button) const { return button == m_Button; }

    private:
        virtual std::string DataToString() const override { return std::format("Controller = {}, Button = {}", m_JID, Utils::ToString(m_Button)); }

    private:
        GamepadButton m_Button;
    };

    class GamepadButtonDownEvent : public GamepadButtonEvent
    {
    public:
        GamepadButtonDownEvent(int gamepadJID, GamepadButton button)
            : GamepadButtonEvent(EventType::GamepadButtonDown, gamepadJID, button)
        {
        }

        static EventType GetStaticType() { return EventType::GamepadButtonDown; }
    };

    class GamepadButtonUpEvent : public GamepadButtonEvent
    {
    public:
        GamepadButtonUpEvent(int gamepadJID, GamepadButton button)
            : GamepadButtonEvent(EventType::GamepadButtonUp, gamepadJID, button)
        {
        }

        static EventType GetStaticType() { return EventType::GamepadButtonUp; }
    };
}