#pragma once

#include "Core/KeyCodes.h"
#include "InputEvent.h"

namespace pxl
{
    class KeyEvent : public InputEvent
    {
    public:
        KeyEvent(EventType type, const std::shared_ptr<InputSystem>& system, KeyCode key, int mods)
            : InputEvent(type, system), m_Key(key), m_Modifiers(mods)
        {
        }

        KeyCode GetKey() const { return m_Key; }
        int GetMods() const { return m_Modifiers; }

        bool IsKey(KeyCode key) const { return key == m_Key; }
        bool IsMods(int mods) const { return (m_Modifiers & mods) == mods; }
        bool IsModsAndKey(int mods, KeyCode key) const { return IsKey(key) && IsMods(mods); }

    protected:
        virtual std::string DataToString() const override { return std::format("Key = {}", Utils::ToString(m_Key)); }

    protected:
        KeyCode m_Key;
        int m_Modifiers;
    };

    class KeyDownEvent : public KeyEvent
    {
    public:
        KeyDownEvent(const std::shared_ptr<InputSystem>& system, KeyCode key, int mods)
            : KeyEvent(EventType::KeyDown, system, key, mods)
        {
        }

        static EventType GetStaticType() { return EventType::KeyDown; }
    };

    class KeyUpEvent : public KeyEvent
    {
    public:
        KeyUpEvent(const std::shared_ptr<InputSystem>& system, KeyCode key, int mods)
            : KeyEvent(EventType::KeyUp, system, key, mods)
        {
        }

        static EventType GetStaticType() { return EventType::KeyUp; }
    };
}