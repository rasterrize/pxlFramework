#pragma once

#include "EventType.h"

namespace pxl
{
    class Event
    {
    public:
        Event(EventType type)
            : m_Type(type)
        {
        }
        virtual ~Event() {}

        EventType GetType() const { return m_Type; }

        std::string ToString() const { return std::format("[{}]: {}", Utils::ToString(m_Type), DataToString()); }

        void Handled() { m_Handled = true; }
        bool IsHandled() { return m_Handled; }

    protected:
        virtual std::string DataToString() const = 0;

    protected:
        bool m_Handled = false;
        EventType m_Type = EventType::Unknown;
    };
}