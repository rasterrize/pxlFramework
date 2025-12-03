#pragma once

#include "Event.h"

namespace pxl
{
    class IEventHandler
    {
    public:
        virtual void OnEvent(Event& e) = 0;
    };
}