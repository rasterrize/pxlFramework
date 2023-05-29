#pragma once

#include "KeyCodes.h"
#include "ButtonCodes.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace pxl
{
    class Input
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }
    
        static bool IsKeyPressed(KeyCode keyCode);
        static bool IsKeyHeld(KeyCode keyCode);

        static bool IsMouseButtonPressed(ButtonCode buttonCode);
        static bool IsMouseButtonHeld(ButtonCode buttonCode);

        static glm::vec2 GetCursorPosition() { return s_CursorPosition; }
        //static int GetKeyState(KeyCode keycode) { return s_KeyStates[keycode]; }
    private:
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    private:
        friend class Window; // for callbacks

        static bool s_Enabled;
        static GLFWwindow* s_WindowHandle;

        static std::unordered_map<int, int> s_CurrentKeyStates;
        static std::unordered_map<int, int> s_PreviousKeyStates;

        static std::unordered_map<int, int> s_CurrentMBStates;
        static std::unordered_map<int, int> s_PreviousMBStates;

        static glm::vec2 s_CursorPosition;
    };
}