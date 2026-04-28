#include "InputSystem.h"

#include "Core/Window.h"
#include "Events/KeyboardEvents.h"
#include "Events/MouseEvents.h"
#include "Platform/Platform.h"

namespace pxl
{
    InputSystem::InputSystem(GLFWwindow* window, const std::function<void(std::unique_ptr<Event>)>& eventCallback)
        : m_Window(window), m_EventCallback(eventCallback)
    {
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
        {
            PXL_PROFILE_SCOPE_NAMED("GLFW Cursor Callback");

            auto& input = static_cast<Window*>(glfwGetWindowUserPointer(window))->GetInputSystem();

            input->m_CurrentInputState.CursorPosition = { xpos, ypos };

            auto event = std::make_unique<MouseMoveEvent>(input, glm::dvec2(xpos, ypos));
            input->m_EventCallback(std::move(event));
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
        {
            PXL_PROFILE_SCOPE_NAMED("GLFW Mouse Button Callback");

            auto& input = static_cast<Window*>(glfwGetWindowUserPointer(window))->GetInputSystem();

            input->m_CurrentInputState.MBStates[static_cast<MouseCode>(button)] = action;

            switch (action)
            {
                case GLFW_PRESS:
                {
                    auto downEvent = std::make_unique<MouseButtonDownEvent>(input, static_cast<MouseCode>(button), mods);
                    input->m_EventCallback(std::move(downEvent));
                    break;
                }
                case GLFW_RELEASE:
                {
                    auto upEvent = std::make_unique<MouseButtonUpEvent>(input, static_cast<MouseCode>(button), mods);
                    input->m_EventCallback(std::move(upEvent));
                    break;
                }
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            PXL_PROFILE_SCOPE_NAMED("GLFW Scroll Callback");

            auto& input = static_cast<Window*>(glfwGetWindowUserPointer(window))->GetInputSystem();

            input->m_CurrentInputState.HorizontalScrollOffset = xoffset;
            input->m_CurrentInputState.VerticalScrollOffset = yoffset;

            auto event = std::make_unique<MouseScrollEvent>(input, yoffset, xoffset);
            input->m_EventCallback(std::move(event));
        });

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int mods)
        {
            PXL_PROFILE_SCOPE_NAMED("GLFW Key Callback");

            auto windowHandle = static_cast<Window*>(glfwGetWindowUserPointer(window));
            auto& input = windowHandle->GetInputSystem();

            input->m_CurrentInputState.KeyStates[static_cast<KeyCode>(key)] = action;

            // Override GLFW's windows key auto iconify so the start menu doesn't immediately disappear
            if (key == GLFW_KEY_LEFT_SUPER && action == GLFW_PRESS && glfwGetWindowMonitor(window) && windowHandle->GetWindowMode() == WindowMode::Fullscreen)
                windowHandle->Minimize();

            switch (action)
            {
                case GLFW_PRESS:
                {
                    auto downEvent = std::make_unique<KeyDownEvent>(input, static_cast<KeyCode>(key), mods);
                    input->m_EventCallback(std::move(downEvent));
                    break;
                }
                case GLFW_RELEASE:
                {
                    auto upEvent = std::make_unique<KeyUpEvent>(input, static_cast<KeyCode>(key), mods);
                    input->m_EventCallback(std::move(upEvent));
                    break;
                }
                case GLFW_REPEAT:
                    auto repeatEvent = std::make_unique<KeyRepeatEvent>(input, static_cast<KeyCode>(key), mods);
                    input->m_EventCallback(std::move(repeatEvent));
                    break;
            }
        });

        m_RawInputSupported = Platform::IsRawInputSupported();
    }

    void InputSystem::ResetCurrentState()
    {
        PXL_PROFILE_SCOPE;

        m_PreviousInputState = m_CurrentInputState;
        m_CurrentInputState.VerticalScrollOffset = 0.0;
        m_CurrentInputState.HorizontalScrollOffset = 0.0;
    }

    void InputSystem::SetRawInput(bool enable)
    {
        if (enable && !m_RawInputSupported)
        {
            PXL_LOG_ERROR(LogArea::Input, "Raw input is not supported on this system");
        }

        glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, enable);
    }
}