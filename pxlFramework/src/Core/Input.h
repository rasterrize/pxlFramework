#pragma once

#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include "KeyCodes.h"
#include "MouseCodes.h"
#include "Window.h"

namespace pxl
{
    enum class CursorMode
    {
        Normal,
        Hidden,
        Disabled,
        Captured,
    };

    enum class StandardCursor
    {
        Arrow,
        IBeam,
        Crosshair,
        Hand,
        HResize,
        VResize,
    };

    class Cursor
    {
    public:
        Cursor(const std::shared_ptr<Image>& image, int32_t hotspotX = 0, int32_t hotspotY = 0)
        {
            GLFWimage glfwImage;
            glfwImage.width = image->Metadata.Size.Width;
            glfwImage.height = image->Metadata.Size.Height;
            glfwImage.pixels = image->Buffer.data();

            m_Cursor = glfwCreateCursor(&glfwImage, hotspotX, hotspotY);
        }

        GLFWcursor* GetNativeCursor() { return m_Cursor; }

    private:
        GLFWcursor* m_Cursor = nullptr;
    };

    class Input
    {
    public:
        static void Init(const std::shared_ptr<Window>& window);
        static void Update();
        static void Shutdown();

        static bool IsInitialized() { return s_Enabled; }

        static bool IsKeyPressed(KeyCode keyCode);
        static bool IsKeyHeld(KeyCode keyCode);

        static bool IsMouseButtonPressed(MouseCode buttonCode);
        static bool IsMouseButtonHeld(MouseCode buttonCode);

        static bool IsMouseScrolledUp();
        static bool IsMouseScrolledDown();

        static const glm::dvec2& GetCursorPosition() { return s_CursorPosition; }

        // NOTE: this functions takes integers but glfw handles cursor positions in screen coordinates
        static void SetCursorPosition(uint32_t x, uint32_t y);

        // Get the cursor distance moved in screen coordinates since last update
        static glm::vec2 GetCursorDelta() { return s_CursorPosition - s_LastCursorPosition; }

        static void SetCursorMode(CursorMode cursorMode);

        static bool GetRawInput();
        static void SetRawInput(bool value);

        static void SetCursorVisibility(bool visible);

        static void SetCursor(StandardCursor standardCursor);
        static void SetCursor(Cursor customCursor);

    private:
        static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    private:
        friend class Window; // for callbacks

        static inline bool s_Enabled = false;
        static inline GLFWwindow* s_WindowHandle = nullptr;

        static inline std::unordered_map<int, int> s_CurrentKeyStates;
        static inline std::unordered_map<int, int> s_PreviousKeyStates;

        static inline std::unordered_map<int, int> s_CurrentMBStates;
        static inline std::unordered_map<int, int> s_PreviousMBStates;

        static inline double s_VerticalScrollOffset = 0.0f;
        static inline double s_HorizontalScrollOffset = 0.0f;

        static inline glm::dvec2 s_CursorPosition = glm::dvec2(0.0f);
        static inline glm::dvec2 s_LastCursorPosition = glm::dvec2(0.0f);

        static inline bool s_RawInputSupported = false;
    };
}