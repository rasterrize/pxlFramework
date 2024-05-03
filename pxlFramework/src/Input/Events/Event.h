#pragma once

namespace pxl
{
    enum class EventType
    {
        None = 0,
        KeyDown,
        KeyUp,
    };

    class Event
    {
    public:
        
    private:
        EventType m_EventType;
    };
}