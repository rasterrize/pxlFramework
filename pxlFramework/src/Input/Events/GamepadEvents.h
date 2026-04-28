#pragma once

#include "Input/Gamepad.h"
#include "Core/Events/Event.h"

namespace pxl
{
    class GamepadEvent : public Event
    {
    protected:
        GamepadEvent(int32_t jid)
            : m_JID(jid)
        {
        }

        int32_t GetGamepadJID() const { return m_JID; }

    protected:
        int32_t m_JID = 0;
    };

    class GamepadAxisChangeEvent : public GamepadEvent
    {
    public:
        GamepadAxisChangeEvent(int32_t jid, GamepadAxis axis, float value)
            : GamepadEvent(jid), m_Axis(axis), m_Value(value)
        {
        }

        static EventType GetStaticType() { return EventType::GamepadAxisChange; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Controller = {}, Axis = {}, Value = {}", m_JID, Utils::ToString(m_Axis), m_Value); }

        GamepadAxis GetAxis() const { return m_Axis; }
        float GetAxisValue() const { return m_Value; }

        bool IsAxis(GamepadAxis axis) const { return axis == m_Axis; }
        bool IsThumbstick() const { return Utils::IsGamepadAxisAThumbstick(m_Axis); }
        bool IsTrigger() const { return Utils::IsGamepadAxisATrigger(m_Axis); }

    private:
        GamepadAxis m_Axis = {};
        float m_Value = 0.0f;
    };

    class GamepadButtonEvent : public GamepadEvent
    {
    protected:
        GamepadButtonEvent(int32_t jid, GamepadButton button)
            : GamepadEvent(jid), m_Button(button)
        {
        }

        virtual std::string DataToString() const override { return std::format("Controller = {}, Button = {}", m_JID, Utils::ToString(m_Button)); }

        GamepadButton GetButton() const { return m_Button; }

        bool IsButton(GamepadButton button) const { return button == m_Button; }

    private:
        GamepadButton m_Button = {};
    };

    class GamepadButtonDownEvent : public GamepadButtonEvent
    {
    public:
        GamepadButtonDownEvent(int32_t jid, GamepadButton button)
            : GamepadButtonEvent(jid, button)
        {
        }

        static EventType GetStaticType() { return EventType::GamepadButtonDown; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
    };

    class GamepadButtonUpEvent : public GamepadButtonEvent
    {
    public:
        GamepadButtonUpEvent(int32_t jid, GamepadButton button)
            : GamepadButtonEvent(jid, button)
        {
        }

        static EventType GetStaticType() { return EventType::GamepadButtonUp; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
    };

    class GamepadStatusChangeEvent : public GamepadEvent
    {
    public:
        GamepadStatusChangeEvent(int32_t jid, GamepadStatus status)
            : GamepadEvent(jid), m_Status(status)
        {
        }

        static EventType GetStaticType() { return EventType::GamepadStatusChange; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual std::string DataToString() const override { return std::format("Controller = {}, Status = {}", m_JID, Utils::ToString(m_Status)); }

        GamepadStatus GetStatus() const { return m_Status; }

        bool WasConnected() const { return m_Status == GamepadStatus::Connected; }
        bool WasDisconnected() const { return m_Status == GamepadStatus::Disconnected; }

    private:
        GamepadStatus m_Status = {};
    };
}