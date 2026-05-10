#pragma once

// clang-format off
// Include volk.h before glfw since we don't want glfw including vulkan.h
#include <volk/volk.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/Image.h"
#include "Core/Size.h"
#include "Input/InputSystem.h"
#include "Monitor.h"

namespace pxl
{
    enum class WindowMode
    {
        Windowed,
        Borderless,
        Fullscreen,
    };

    namespace WindowConstants
    {
        static const Size2D DefaultWindowedSize = { 1280, 720 };
        static const glm::ivec2 DefaultWindowedPosition = { INT32_MAX, INT32_MAX };
        static const std::string DefaultWindowTitle = "Default Window Title";
        static const WindowMode DefaultWindowMode = WindowMode::Windowed;
        static const bool DefaultShowOnceRendererIsWorking = true;
    }

    struct WindowSpecs
    {
        // The title of the window (appears top left of window).
        std::string Title = WindowConstants::DefaultWindowTitle;

        // The size of the window in screen coordinates. Only used for Windowed mode. If not specified, defaults to k_DefaultWindowedSize.
        Size2D WindowedSize = WindowConstants::DefaultWindowedSize;

        // The position of the window once it's windowed. Default value sets the window to the center of the primary monitor.
        glm::ivec2 WindowedPosition = WindowConstants::DefaultWindowedPosition;

        // The window mode of the window. Defaults to Windowed.
        WindowMode WindowMode = WindowConstants::DefaultWindowMode;

        // The monitor to use for Borderless and Fullscreen modes. If not supplied, defaults to the primary monitor.
        std::optional<uint8_t> FullscreenMonitorIndex;

        std::optional<Size2D> FullscreenResolution;
        std::optional<VideoMode> FullscreenVideoMode;

        std::optional<uint32_t> FullscreenRefreshRate;

        // The path to load an icon from for this window. If not supplied, the window will have no icon.
        std::optional<std::string> IconPath;

        // Show the window only once the renderer is presenting. This helps avoid the flashing white as the renderer initializes.
        bool ShowOnceRendererIsWorking = WindowConstants::DefaultShowOnceRendererIsWorking;
    };

    /// @brief A desktop window used to display stuff to the user.
    class Window
    {
    public:
        Window(const WindowSpecs& specs);

        void Close() const;

        const glm::ivec2& GetPosition() const { return m_Position; }
        void SetPosition(int32_t x, int32_t y);

        const glm::ivec2& GetLastWindowedPosition() const { return m_LastWindowedPosition; }

        WindowMode GetWindowMode() const { return m_WindowMode; }
        void SetWindowMode(WindowMode mode);

        /// @brief Moves the window to the specified monitor.
        /// @param monitor A valid monitor object.
        void SetMonitor(const Monitor& monitor);

        Size2D GetSize() const { return m_Size; }
        void SetSize(const Size2D& size) { glfwSetWindowSize(m_GLFWWindow, size.Width, size.Height); }

        /// @brief Sets limits for the dimensions of the window.
        /// @note To avoid setting a minimum or maximum, use a size of -1 for both width and height.
        void SetSizeLimits(const Size2D& minSize, const Size2D& maxSize);

        Size2D GetLastWindowedSize() const { return m_LastWindowedSize; }

        /// @brief Gets a floating point representation of the aspect ratio of this window (e.g 16:9 returns 1.7777)
        /// by dividing the width and height.
        float GetAspectRatio() const { return static_cast<float>(m_Size.Width) / static_cast<float>(m_Size.Height); }

        Size2D GetFramebufferSize() const;

        /// @brief Switches to the next window mode in the order of Windowed -> Borderless -> Fullscreen.
        /// This function will automatically wrap around from Fullscreen to Windowed.
        void NextWindowMode();

        /// @brief Toggles between Fullscreen and Windowed window modes.
        /// @note This function will always switch to exclusive fullscreen mode from windowed mode.
        void ToggleFullscreen();

        void Minimize() const { glfwIconifyWindow(m_GLFWWindow); }

        void Maximize() const { glfwMaximizeWindow(m_GLFWWindow); }

        /// @brief Unminimizes a minimized window.
        void Restore() const { glfwRestoreWindow(m_GLFWWindow); }

        bool IsVisible() const { return glfwGetWindowAttrib(m_GLFWWindow, GLFW_VISIBLE); }

        bool IsMinimized() const { return glfwGetWindowAttrib(m_GLFWWindow, GLFW_ICONIFIED); }

        void Show() const { glfwShowWindow(m_GLFWWindow); }

        void Hide() const { glfwHideWindow(m_GLFWWindow); }

        std::string_view GetTitle() const { return glfwGetWindowTitle(m_GLFWWindow); }
        void SetTitle(std::string_view title) { glfwSetWindowTitle(m_GLFWWindow, title.data()); }

        void SetIcon(Image& image);

        bool IsFocused() const { return glfwGetWindowAttrib(m_GLFWWindow, GLFW_FOCUSED); }

        void SetCursorMode(CursorMode mode);

        void SetCursor(StandardCursor cursor) { glfwSetCursor(m_GLFWWindow, glfwCreateStandardCursor(Utils::ToGLFWStandardCursor(cursor))); }
        void SetCursor(Cursor cursor) { glfwSetCursor(m_GLFWWindow, cursor.GetNativeCursor()); }
        void ResetCursor() { glfwSetCursor(m_GLFWWindow, nullptr); }

        bool WillShowOnceRendererIsWorking() const { return m_ShowOnceRendererIsWorking; }

        /// @brief Enforces an aspect ratio for the window's dimensions.
        /// @param numerator The width value of the ratio.
        /// @param denominator The height value of the ratio.
        void EnforceAspectRatio(uint32_t numerator, uint32_t denominator) const;

        /// @brief Requests the users attention by highlighting the application's icon in the operating system taskbar.
        void RequestUserAttention() const { glfwRequestWindowAttention(m_GLFWWindow); }

        /// @brief Get the current monitor this window is on
        /// @return The window's current monitor
        const Monitor& GetCurrentMonitor() const { return m_CurrentMonitor; }

        /// @brief Creates a Vulkan surface for this window.
        /// @note This surface will NOT be automatically destroyed by the window class.
        VkSurfaceKHR CreateVKSurface(VkInstance instance);

        /// @brief Gets the underlying window system API handle of this window.
        /// @return The GLFWwindow handle of this window.
        GLFWwindow* GetNativeWindow() const { return m_GLFWWindow; }

        const std::shared_ptr<InputSystem>& GetInputSystem() const { return m_InputSystem; }

    public:
        /// @brief Creates and prepares a new window object. Do not try to create windows another way as things will break.
        /// @param windowSpecs The specifications for the window.
        /// @return The new window.
        [[nodiscard]] static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);

        static void CloseAll();

    private:
        void InitCallbacks();
        void DetectCurrentMonitor();

    private:
        friend class InputSystem;

        // Per-window GLFW callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
        static void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos);
        static void DropCallback(GLFWwindow* window, int count, const char** paths);
        static void CursorEnterCallback(GLFWwindow* window, int entered);

        friend class Application; // for the below functions
        static void ResetPerFrameState();

    private:
        GLFWwindow* m_GLFWWindow = nullptr;
        std::weak_ptr<Window> m_SelfHandle;
        std::shared_ptr<InputSystem> m_InputSystem;
        std::function<void(Event&)> m_EventCallback;

        Size2D m_Size = WindowConstants::DefaultWindowedSize;
        glm::ivec2 m_Position = WindowConstants::DefaultWindowedPosition;
        WindowMode m_WindowMode = WindowConstants::DefaultWindowMode;

        // The size and position of the window when it was in windowed mode
        glm::ivec2 m_LastWindowedPosition = WindowConstants::DefaultWindowedPosition;
        Size2D m_LastWindowedSize = WindowConstants::DefaultWindowedSize;

        // The current monitor this window is on
        Monitor m_CurrentMonitor = {};

        Monitor m_FullscreenMonitor = {};
        VideoMode* m_FullscreenVideoMode = nullptr;

        bool m_ShowOnceRendererIsWorking = WindowConstants::DefaultShowOnceRendererIsWorking;

    private:
        // Storage of all windows
        static inline std::vector<std::shared_ptr<Window>> s_Windows;
    };

    namespace Utils
    {
        inline std::string ToString(WindowMode mode)
        {
            switch (mode)
            {
                case WindowMode::Windowed:   return "Windowed";
                case WindowMode::Borderless: return "Borderless";
                case WindowMode::Fullscreen: return "Fullscreen";
                default:                     return "Unknown";
            }
        }

        inline WindowMode ToWindowMode(std::string_view modeString)
        {
            const std::unordered_map<std::string, WindowMode> stringToWindowMode = {
                {   Utils::ToString(WindowMode::Windowed),   WindowMode::Windowed },
                { Utils::ToString(WindowMode::Borderless), WindowMode::Borderless },
                { Utils::ToString(WindowMode::Fullscreen), WindowMode::Fullscreen },
            };

            for (auto& [string, mode] : stringToWindowMode)
            {
                if (modeString == string)
                    return mode;
            }

            PXL_ASSERT_MSG(false, "Failed to get window mode from string");
            return WindowMode::Windowed;
        }
    }
}