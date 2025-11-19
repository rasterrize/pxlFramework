#pragma once

#include <GLFW/glfw3.h>

#include "Events/Event.h"
#include "GamepadCodes.h"

namespace pxl
{
    class Gamepad
    {
    public:
        Gamepad(uint32_t jid, const std::function<void(std::unique_ptr<pxl::Event>)>& eventCallback);

        bool IsButtonHeld(GamepadButton button);

        float GetAxis(GamepadAxis axis);
        std::string GetName();
        std::string GetGUID();

        void UpdateState();

    private:
        int32_t m_JID = 0;
        std::function<void(std::unique_ptr<pxl::Event>)> m_EventCallback;

        GLFWgamepadstate m_State;
        GLFWgamepadstate m_PreviousState;

        // NOTE: used to avoid propagating events on the first update cycle
        bool m_FirstStateCheck = true;
    };
}