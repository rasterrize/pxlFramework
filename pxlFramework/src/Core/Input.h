#pragma once

#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include "Gamepad.h"
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
        static void Shutdown();

        static bool IsInitialized() { return s_Enabled; }

        static bool IsKeyPressed(KeyCode keyCode);
        static bool IsKeyHeld(KeyCode keyCode);

        static bool IsMouseButtonPressed(MouseCode buttonCode);
        static bool IsMouseButtonHeld(MouseCode buttonCode);

        static bool IsMouseScrolledUp();
        static bool IsMouseScrolledDown();

        static const glm::dvec2& GetCursorPosition() { return s_InputSystem->GetCurrentState().CursorPosition; }

        // NOTE: this functions takes integers but glfw handles cursor positions in screen coordinates
        static void SetCursorPosition(double x, double y);

        // Get the cursor distance moved in screen coordinates since last update
        static glm::vec2 GetCursorDelta();

        static void SetCursorMode(CursorMode cursorMode);

        static bool GetRawInput();
        static void SetRawInput(bool value);

        static void SetCursorVisibility(bool visible);

        static void SetCursor(StandardCursor standardCursor);
        static void SetCursor(Cursor customCursor);

    private:
        friend class Window; // for callbacks

        static inline bool s_Enabled = false;
        static inline GLFWwindow* s_WindowHandle = nullptr;
        static inline std::shared_ptr<InputSystem> s_InputSystem;

        static inline bool s_RawInputSupported = false;
    };
}