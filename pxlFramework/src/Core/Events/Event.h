#pragma once

#include "EventType.h"

namespace pxl
{
    struct Event
    {
        bool Handled = false;

        virtual ~Event() = default;

        virtual EventType GetEventType() const = 0;

        std::string ToString() const { return std::format("[{}]: {}", Utils::ToString(GetEventType()), DataToString()); }
        virtual std::string DataToString() const = 0;
    };
}