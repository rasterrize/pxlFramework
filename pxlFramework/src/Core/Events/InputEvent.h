#pragma once

#include "Core/InputSystem.h"

namespace pxl
{
    class InputEvent : public Event
    {
    public:
        InputEvent(EventType type, const std::shared_ptr<InputSystem>& system)
            : Event(type), m_InputSystem(system)
        {
        }

        // Get the current global InputState. Useful for checking state on a different input at the same time this even is received.
        const InputState& GetInputState() const { return m_InputSystem.lock()->GetCurrentState(); }

        // Get the previous global InputState. Useful for checking state on a different input at the same time this even is received.
        const InputState& GetPreviousInputState() const { return m_InputSystem.lock()->GetPreviousState(); }

    protected:
        std::weak_ptr<InputSystem> m_InputSystem;
    };
}