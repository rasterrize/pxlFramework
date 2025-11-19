#include "Gamepad.h"

#include <GLFW/glfw3.h>

#include "Events/GamepadEvents.h"

namespace pxl
{
    Gamepad::Gamepad(uint32_t jid, const std::function<void(std::unique_ptr<pxl::Event>)>& eventCallback)
        : m_JID(jid), m_EventCallback(eventCallback)
    {
        UpdateState();
    }

    bool Gamepad::IsButtonHeld(GamepadButton button)
    {
        return m_State.buttons[static_cast<int32_t>(button)] == GLFW_PRESS;
    }

    float Gamepad::GetAxis(GamepadAxis axis)
    {
        return m_State.axes[static_cast<int32_t>(axis)];
    }

    std::string Gamepad::GetName()
    {
        return glfwGetGamepadName(m_JID);
    }

    std::string Gamepad::GetGUID()
    {
        return glfwGetJoystickGUID(m_JID);
    }

    void Gamepad::UpdateState()
    {
        m_PreviousState = m_State;

        GLFWgamepadstate state;
        glfwGetGamepadState(m_JID, &state);

        m_State = state;

        if (m_FirstStateCheck)
        {
            m_FirstStateCheck = false;
            return;
        }

        // Propogate axes events
        auto axesCount = sizeof(m_State.axes) / sizeof(m_State.axes[0]);
        for (size_t i = 0; i < axesCount; i++)
        {
            auto axisValue = m_State.axes[i];

            if (axisValue == m_PreviousState.axes[i])
                continue;

            auto axisChangeEvent = std::make_unique<GamepadAxisChangeEvent>(m_JID, static_cast<GamepadAxis>(i), axisValue);
            m_EventCallback(std::move(axisChangeEvent));
        }

        // Propogate button events
        auto buttonCount = sizeof(m_State.buttons) / sizeof(m_State.buttons[0]);
        for (size_t i = 0; i < buttonCount; i++)
        {
            auto buttonState = m_State.buttons[i];
            if (buttonState == m_PreviousState.buttons[i])
                continue;

            switch (buttonState)
            {
                case GLFW_PRESS:
                {
                    auto downEvent = std::make_unique<GamepadButtonDownEvent>(m_JID, static_cast<GamepadButton>(i));
                    m_EventCallback(std::move(downEvent));
                    break;
                }
                case GLFW_RELEASE:
                {
                    auto upEvent = std::make_unique<GamepadButtonUpEvent>(m_JID, static_cast<GamepadButton>(i));
                    m_EventCallback(std::move(upEvent));
                    break;
                }
            }
        }
    }
}