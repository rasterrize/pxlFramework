#pragma once

#include <GLFW/glfw3.h>

#include "Events/Event.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

namespace pxl
{
    struct InputState
    {
        std::unordered_map<KeyCode, int> KeyStates;
        std::unordered_map<MouseCode, int> MBStates;

        double VerticalScrollOffset = 0.0f;
        double HorizontalScrollOffset = 0.0f;

        glm::dvec2 CursorPosition = glm::dvec2(0.0f);
    };

    class InputSystem
    {
    public:
        InputSystem(GLFWwindow* window, const std::function<void(std::unique_ptr<Event>)>& eventCallback);

        void ResetCurrentState();

        const InputState& GetCurrentState() const { return m_CurrentInputState; }
        const InputState& GetPreviousState() const { return m_PreviousInputState; }

    private:
        GLFWwindow* m_Window = nullptr;

        InputState m_CurrentInputState;
        InputState m_PreviousInputState;

        std::function<void(std::unique_ptr<Event>)> m_EventCallback = nullptr;
    };
}