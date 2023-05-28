#pragma once

#include "KeyCodes.h"
#include "Window.h"
#include <GLFW/glfw3.h>

namespace pxl
{
    class Input
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }
    
        static bool IsKeyPressed(KeyCode keycode);
        static bool IsKeyHeld(KeyCode keycode);
        //static int GetKeyState(KeyCode keycode) { return s_KeyStates[keycode]; }
        
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
    private:
        static bool s_Enabled;
        static GLFWwindow* s_WindowHandle;

        static std::unordered_map<int, int> s_CurrentKeyStates;
        static std::unordered_map<int, int> s_PreviousKeyStates;
    };
}