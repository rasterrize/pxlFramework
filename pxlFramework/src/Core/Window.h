#pragma once

// clang-format off
// Include volk.h before glfw since we don't want glfw including vulkan.h
#include <volk/volk.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>

#include "Core/Image.h"
#include "Core/Size.h"
#include "Gamepad.h"
#include "InputSystem.h"

namespace pxl
{
    namespace WindowConstants
    {
        inline constexpr Size2D k_DefaultWindowedSize = { 1280, 720 };
        inline constexpr std::string_view k_DefaultWindowTitle = "Default Window Title";
    }

    enum class WindowMode
    {
        Windowed,
        Borderless,
        Fullscreen,
    };

    struct VideoMode
    {
        VideoMode(const GLFWvidmode* glfwMode)
        {
            Width = glfwMode->width;
            Height = glfwMode->height;
            RefreshRate = glfwMode->refreshRate;
            BitDepth = { glfwMode->redBits, glfwMode->greenBits, glfwMode->blueBits };
            GLFWVidMode = glfwMode;
        }

        Size2D GetSize() { return { Width, Height }; }

        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t RefreshRate = 0;
        glm::ivec3 BitDepth = glm::ivec3(0);
        const GLFWvidmode* GLFWVidMode = nullptr;
    };

    struct Monitor
    {
        uint8_t Index = 1; // refers to the operating system's ID for the monitor
        std::string Name;
        glm::ivec2 Position;
        std::vector<VideoMode> VideoModes;
        Size2D PhysicalSize;
        bool IsPrimary = false;
        GLFWmonitor* GLFWMonitor = nullptr;

        VideoMode GetCurrentVideoMode() const { return VideoMode(glfwGetVideoMode(GLFWMonitor)); }
    };

    struct WindowSpecs
    {
        // The title of the window (appears top left of window).
        std::string Title = WindowConstants::k_DefaultWindowTitle.data();

        // The size of the window in screen coordinates. Only used for Windowed mode. If not specified, defaults to k_DefaultWindowedSize.
        Size2D Size = WindowConstants::k_DefaultWindowedSize;

        // The position of the window. Only used for Windowed mode. If not supplied, defaults to the center of Monitor.
        std::optional<glm::ivec2> Position;

        // The window mode of the window. Defaults to Windowed.
        WindowMode WindowMode = WindowMode::Windowed;

        // The monitor to use for Borderless and Fullscreen modes. If not supplied, defaults to the primary monitor.
        std::optional<uint8_t> MonitorIndex;

        // The path to load an icon from for this window. If not supplied, the window will have no icon.
        std::optional<std::string> IconPath;

        // Use dark mode for window title
        bool DarkMode = true;
    };

    /// @brief A desktop window used to display stuff to the user.
    class Window
    {
    public:
        /// @brief Closes the window. If this is the only window left, the application closes automatically.
        void Close() const;

        const glm::ivec2& GetPosition() const { return m_Position; }
        void SetPosition(int32_t x, int32_t y);

        WindowMode GetWindowMode() const { return m_WindowMode; }
        void SetWindowMode(WindowMode mode);

        /// @brief Moves the window to the specified monitor.
        /// @param index The index of the monitor determined by the OS.
        void SetMonitor(uint8_t index);

        /// @brief Moves the window to the specified monitor.
        /// @param monitor A valid monitor object.
        void SetMonitor(const Monitor& monitor);

        /// @brief Gets the underlying window system API handle of this window.
        /// @return The GLFWwindow handle of this window.
        GLFWwindow* GetNativeWindow() const { return m_GLFWWindow; }

        Size2D GetSize() const { return m_Size; }
        void SetSize(uint32_t width, uint32_t height);

        /// @brief Sets limits for the dimensions of the window.
        /// @param minWidth Minimum window width in screen coordinates.
        /// @param minHeight Minimum window height in screen coordinates.
        /// @param maxWidth Maximum window width in screen coordinates.
        /// @param maxHeight Maximum window height in screen coordinates.
        /// @note You can specify just a minimum or maximum by using -1 for width AND height of the opposite pair.
        void SetSizeLimits(uint32_t minWidth, uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight);

        /// @brief Gets a floating point representation of the aspect ratio of this window (e.g 16:9 returns 1.7777)
        /// by dividing the width and height.
        /// @return The aspect ratio of the window.
        float GetAspectRatio() const { return static_cast<float>(m_Size.Width) / static_cast<float>(m_Size.Height); }

        Size2D GetFramebufferSize() const;

        /// @brief Switches to the next window mode in the order of Windowed -> Borderless -> Fullscreen.
        /// This function will automatically wrap around from Fullscreen to Windowed.
        void NextWindowMode();

        /// @brief Toggles between Fullscreen and Windowed window modes.
        /// @note This function will always switch to exclusive fullscreen mode from windowed mode.
        void ToggleFullscreen();

        /// @brief Minimizes the window.
        void Minimize() const;

        /// @brief Maximizes the window.
        void Maximize() const;

        /// @brief Restores the window if it's minimized.
        void Restore() const;

        /// @brief Gets the visibility status of this window.
        /// @return True if visible, false if hidden.
        bool GetVisibility() const;

        /// @brief Shows the window.
        void Show() const;

        /// @brief Hides the window.
        void Hide() const;

        const std::string& GetTitle() const { return m_Title; }
        void SetTitle(const std::string_view& title);

        void SetIcon(const std::shared_ptr<Image>& image);

        /// @brief Enforces an aspect ratio for the window's dimensions.
        /// @param numerator The width value of the ratio.
        /// @param denominator The height value of the ratio.
        void EnforceAspectRatio(uint32_t numerator, uint32_t denominator) const;

        /// @brief Requests the users attention by highlighting the application's icon in the operating system taskbar.
        void RequestUserAttention() const;

        const Monitor& GetCurrentMonitor() { return m_CurrentMonitor; }

        /// @brief Creates a Vulkan surface for this window.
        /// @note This surface will NOT be automatically destroyed by the window class.
        VkSurfaceKHR CreateVKSurface(VkInstance instance);

    public:
        /// @brief Creates and prepares a new window object. Do not try to create windows another way as things will break.
        /// @param windowSpecs The specifications for the window.
        /// @return The new window.
        [[nodiscard]] static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);

        static const std::vector<Monitor>& GetAvailableMonitors() { return s_Monitors; }

        static void CloseAll();

        static const Monitor& GetPrimaryMonitor();

        static std::vector<const char*> GetVKRequiredInstanceExtensions();

        static bool IsInitialized() { return s_Initialized; }

        static std::shared_ptr<Gamepad> GetGamepad(int id);

    private:
        Window(const WindowSpecs& specs);

        void Update();

        void UpdateCurrentMonitor();

        void InitWindowCallbacks();

    private:
        static void InitGLFWCallbacks();

        friend class InputSystem;
        friend class Input;
        const std::shared_ptr<InputSystem>& GetInputSystem() const { return m_InputSystem; }

        // Per-window GLFW callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
        static void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos);
        static void DropCallback(GLFWwindow* window, int count, const char** paths);

        // Static GLFW callbacks
        static void GLFWErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int event);
        static void JoystickCallback(int jid, int event);

        friend class Application; // for the below functions
        static void Init();
        static void ProcessEvents();
        static void UpdateAll();
        static void Shutdown();

        static void UpdateMonitors();
        static void PrepareConnectedGamepad(int jid);

    private:
        GLFWwindow* m_GLFWWindow = nullptr;
        std::weak_ptr<Window> m_Handle;
        std::shared_ptr<InputSystem> m_InputSystem;
        std::function<void(Event&)> m_EventCallback;

        Size2D m_Size = WindowConstants::k_DefaultWindowedSize;
        glm::ivec2 m_Position = { 0, 0 };
        std::string m_Title = WindowConstants::k_DefaultWindowTitle.data();
        WindowMode m_WindowMode = WindowMode::Windowed;

        bool m_Minimized = false;

        // The size and position of the window when it was in windowed mode
        glm::ivec2 m_LastWindowedPosition = { 0, 0 };
        Size2D m_LastWindowedSize = WindowConstants::k_DefaultWindowedSize;

        // The current monitor this window is on
        Monitor m_CurrentMonitor;

        bool m_ShowAfterFirstPresent = true;

    private:
        static inline bool s_Initialized = false;

        // Storage of all windows and monitors
        static inline std::vector<std::shared_ptr<Window>> s_Windows;
        static inline std::vector<Monitor> s_Monitors;
        static inline std::unordered_map<int, std::shared_ptr<Gamepad>> s_Gamepads;

        // The function to process window events every application update
        static inline std::function<void()> s_EventProcessFunc = glfwPollEvents;
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
    }
}