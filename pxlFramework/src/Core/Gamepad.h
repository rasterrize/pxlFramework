#pragma once

#include <GLFW/glfw3.h>

#include "Events/Event.h"
#include "GamepadCodes.h"

namespace pxl
{
    enum class GamepadStatus
    {
        Connected,
        Disconnected,
    };

    class Gamepad
    {
    public:
        Gamepad(uint32_t jid, const std::function<void(std::unique_ptr<pxl::Event>)>& eventCallback);

        bool IsButtonHeld(GamepadButton button) const;

        float GetAxisValue(GamepadAxis axis) const;
        std::string GetName() const;
        std::string GetGUID() const;

        void UpdateState();

        /// @brief Set the thumbstick deadzone for this gamepad.
        /// @param deadzone The minimum axis value to allow propogating events.
        void SetThumbstickDeadzone(float deadzone) { m_ThumbstickDeadzone = deadzone; }

    private:
        int32_t m_JID = 0;
        std::function<void(std::unique_ptr<pxl::Event>)> m_EventCallback;

        GLFWgamepadstate m_State = {};
        GLFWgamepadstate m_PreviousState = {};

        // NOTE: used to avoid propagating events on the first update cycle
        bool m_FirstStateCheck = true;

        float m_ThumbstickDeadzone = 0.1f;
    };

    namespace Utils
    {
        inline std::string ToString(GamepadStatus status)
        {
            switch (status)
            {
                case GamepadStatus::Connected:    return "Connected";
                case GamepadStatus::Disconnected: return "Disconnected";
                default:                          return "Unknown";
            }
        }

        inline bool IsGamepadAxisAThumbstick(GamepadAxis axis)
        {
            switch (axis)
            {
                case GamepadAxis::LeftThumbX:  return true;
                case GamepadAxis::LeftThumbY:  return true;
                case GamepadAxis::RightThumbX: return true;
                case GamepadAxis::RightThumbY: return true;
                default:                       return false;
            }
        }

        inline bool IsGamepadAxisATrigger(GamepadAxis axis)
        {
            switch (axis)
            {
                case GamepadAxis::LeftTrigger:  return true;
                case GamepadAxis::RightTrigger: return true;
                default:                        return false;
            }
        }
    }
}