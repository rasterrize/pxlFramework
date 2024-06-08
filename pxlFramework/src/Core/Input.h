#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Window.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

namespace pxl
{
    enum class CursorMode
    {
        Normal = 0, Hidden, Disabled
    };

    class Input
    {
    public:
        static void Init(const std::shared_ptr<Window>& window);
        static void Shutdown();

        static const bool IsInitialized() { return s_Enabled; }
    
        static bool IsKeyPressed(KeyCode keyCode);
        static bool IsKeyHeld(KeyCode keyCode);

        static bool IsMouseButtonPressed(MouseCode buttonCode);
        static bool IsMouseButtonHeld(MouseCode buttonCode);

        static bool IsMouseScrolledUp();
        static bool IsMouseScrolledDown();

        static const glm::vec2& GetCursorPosition() { return s_CursorPosition; }
        static void SetCursorPosition(uint32_t x, uint32_t y);
        static void SetCursorMode(CursorMode cursorMode);
        static void SetRawInput(bool value);

        static void SetCursorVisibility(bool visible);
    private:
        static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    private:
        friend class Window; // for callbacks

        static bool s_Enabled;
        static GLFWwindow* s_WindowHandle;

        static std::unordered_map<int, int> s_CurrentKeyStates;
        static std::unordered_map<int, int> s_PreviousKeyStates;

        static std::unordered_map<int, int> s_CurrentMBStates;
        static std::unordered_map<int, int> s_PreviousMBStates;

        static double s_VerticalScrollOffset;
        static double s_HorizontalScrollOffset;

        static glm::vec2 s_CursorPosition;
    };
}