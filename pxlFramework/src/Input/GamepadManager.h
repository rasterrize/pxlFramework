#pragma once

#include "Core/Events/Event.h"
#include "Gamepad.h"

namespace pxl
{
    class GamepadManager
    {
    public:
        GamepadManager(const std::function<void(std::unique_ptr<Event>)>& eventCallback);

        void ProcessStateChanges();

        std::shared_ptr<Gamepad> GetGamepad(int32_t id) const { return m_Gamepads.at(id); }

    private:
        void PrepareGamepad(int32_t id, const std::function<void(std::unique_ptr<Event>)>& eventCallback);

        static void JoystickCallback(int jid, int event);

    private:
        std::function<void(std::unique_ptr<Event>)> m_EventCallback;

        std::array<std::shared_ptr<Gamepad>, GLFW_JOYSTICK_LAST + 1> m_Gamepads;
    };
}