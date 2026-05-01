#include "GamepadManager.h"

#include <GLFW/glfw3.h>

#include "Core/Application.h"
#include "Events/GamepadEvents.h"

namespace pxl
{
    GamepadManager::GamepadManager(const std::function<void(std::unique_ptr<Event>)>& eventCallback)
        : m_EventCallback(eventCallback)
    {
        for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
        {
            if (glfwJoystickIsGamepad(i))
            {
                PrepareGamepad(i, m_EventCallback);
            }
        }

        glfwSetJoystickCallback(JoystickCallback);
    }

    void GamepadManager::ProcessStateChanges()
    {
        for (auto& gamepad : m_Gamepads)
        {
            if (gamepad)
                gamepad->UpdateState();
        }
    }

    void GamepadManager::PrepareGamepad(int32_t id, const std::function<void(std::unique_ptr<Event>)>& eventCallback)
    {
        m_Gamepads[id] = std::make_shared<Gamepad>(id, eventCallback);

        PXL_LOG_INFO("Controller {} ({}) connected", id, m_Gamepads.at(id)->GetName());
    }

    void GamepadManager::JoystickCallback(int jid, int event)
    {
        // FIXME: It's impossible to get the gamepad manager using glfwGetJoystickUserPointer as glfw resets it.
        // As a temporary solution, we'll get the application's one instead, but this assumes only one exists ever
        auto& manager = Application::Get().GetGamepadManager();

        if (event == GLFW_CONNECTED)
        {
            if (glfwJoystickIsGamepad(jid))
            {
                manager.PrepareGamepad(jid, manager.m_EventCallback);
                auto statusEvent = std::make_unique<GamepadStatusChangeEvent>(jid, GamepadStatus::Connected);
                manager.m_EventCallback(std::move(statusEvent));
            }
            else
            {
                PXL_LOG_WARN("Connected controller wasn't detected as a suitable gamepad");
            }
        }
        else if (event == GLFW_DISCONNECTED)
        {
            PXL_LOG_INFO("Controller {} ({}) disconnected", jid, manager.m_Gamepads.at(jid)->GetName());
            auto statusEvent = std::make_unique<GamepadStatusChangeEvent>(jid, GamepadStatus::Disconnected);
            manager.m_EventCallback(std::move(statusEvent));
            manager.m_Gamepads.at(jid).reset();
        }
    }
}