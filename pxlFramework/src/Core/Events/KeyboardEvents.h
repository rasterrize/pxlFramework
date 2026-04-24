#pragma once

#include "Core/KeyCodes.h"
#include "InputEvent.h"

namespace pxl
{
    class KeyEvent : public InputEvent
    {
    public:
        KeyEvent(const std::shared_ptr<InputSystem>& system, KeyCode key, KeyModFlags mods)
            : InputEvent(system), m_Key(key), m_Modifiers(mods)
        {
        }

        virtual std::string DataToString() const override { return std::format("Key = {}", Utils::ToString(m_Key)); }

        KeyCode GetKey() const { return m_Key; }
        KeyModFlags GetMods() const { return m_Modifiers; }

        bool IsKey(KeyCode key) const { return key == m_Key; }
        bool IsMods(KeyModFlags mods) const { return (m_Modifiers & mods) == mods; }
        bool IsModsAndKey(KeyModFlags mods, KeyCode key) const { return IsMods(mods) && IsKey(key); }

    private:
        KeyCode m_Key = {};
        KeyModFlags m_Modifiers = 0;
    };

    class KeyDownEvent : public KeyEvent
    {
    public:
        KeyDownEvent(const std::shared_ptr<InputSystem>& system, KeyCode key, KeyModFlags mods)
            : KeyEvent(system, key, mods)
        {
        }

        static EventType GetStaticType() { return EventType::KeyDown; }
        EventType GetEventType() const { return GetStaticType(); }
    };

    class KeyUpEvent : public KeyEvent
    {
    public:
        KeyUpEvent(const std::shared_ptr<InputSystem>& system, KeyCode key, KeyModFlags mods)
            : KeyEvent(system, key, mods)
        {
        }

        static EventType GetStaticType() { return EventType::KeyUp; }
        EventType GetEventType() const { return GetStaticType(); }
    };

    // This event is intended for text input, not continuous 'held-down' processing
    class KeyRepeatEvent : public KeyEvent
    {
    public:
        KeyRepeatEvent(const std::shared_ptr<InputSystem>& system, KeyCode key, KeyModFlags mods)
            : KeyEvent(system, key, mods)
        {
        }

        static EventType GetStaticType() { return EventType::KeyRepeat; }
        EventType GetEventType() const { return GetStaticType(); }
    };
}