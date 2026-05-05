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
        static const std::string_view DefaultWindowTitle = "Default Window Title";
        static const WindowMode DefaultWindowMode = WindowMode::Windowed;
        static const bool DefaultShowOnceRendererIsWorking = true;
    }

    struct VideoMode
    {
        VideoMode(const GLFWvidmode* glfwMode)
            : Size(glfwMode->width, glfwMode->height),
              RefreshRate(glfwMode->refreshRate),
              BitDepth(glfwMode->redBits, glfwMode->greenBits, glfwMode->blueBits),
              GLFWVidMode(glfwMode)
        {
        }

        std::string ToString() { return std::format("{}x{}, {}hz", Size.Width, Size.Height, RefreshRate); }

        Size2D Size = {};
        uint32_t RefreshRate = 0;
        glm::ivec3 BitDepth = {};
        const GLFWvidmode* GLFWVidMode = nullptr;
    };

    struct Monitor
    {
        uint8_t Index = 1; // refers to the operating system's ID for the monitor
        std::string Name = "Unnamed Monitor";
        glm::ivec2 Position = {};
        std::vector<VideoMode> VideoModes;
        Size2D PhysicalSize = {};
        bool IsPrimary = false;
        GLFWmonitor* GLFWMonitor = nullptr;

        VideoMode GetCurrentVideoMode() const { return VideoMode(glfwGetVideoMode(GLFWMonitor)); }
    };

    struct WindowSpecs
    {
        // The title of the window (appears top left of window).
        std::string Title = WindowConstants::DefaultWindowTitle.data();

        // The size of the window in screen coordinates. Only used for Windowed mode. If not specified, defaults to k_DefaultWindowedSize.
        Size2D Size = WindowConstants::DefaultWindowedSize;

        // The position of the window. Only used for Windowed mode. If not supplied, defaults to the center of Monitor.
        std::optional<glm::ivec2> Position;

        // The window mode of the window. Defaults to Windowed.
        WindowMode WindowMode = WindowMode::Windowed;

        // The monitor to use for Borderless and Fullscreen modes. If not supplied, defaults to the primary monitor.
        std::optional<uint8_t> MonitorIndex;

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

        Size2D GetSize() const { return m_Size; }
        void SetSize(const Size2D& size) { glfwSetWindowSize(m_GLFWWindow, size.Width, size.Height); }

        /// @brief Sets limits for the dimensions of the window.
        /// @note To avoid setting a minimum or maximum, use a size of -1 for both width and height.
        void SetSizeLimits(const Size2D& minSize, const Size2D& maxSize);

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

        void Minimize() const;

        void Maximize() const;

        /// @brief Restores the window if it's minimized.
        void Restore() const;

        bool IsVisible() const;

        void Show() const;

        void Hide() const;

        std::string_view GetTitle() const { return glfwGetWindowTitle(m_GLFWWindow); }
        void SetTitle(std::string_view title);

        void SetIcon(Image& image);

        bool IsFocused() const;

        void SetCursorMode(CursorMode mode);

        void SetCursor(StandardCursor cursor);
        void SetCursor(Cursor cursor);
        void ResetCursor();

        bool WillShowOnceRendererIsWorking() const { return m_ShowOnceRendererIsWorking; }

        /// @brief Enforces an aspect ratio for the window's dimensions.
        /// @param numerator The width value of the ratio.
        /// @param denominator The height value of the ratio.
        void EnforceAspectRatio(uint32_t numerator, uint32_t denominator) const;

        /// @brief Requests the users attention by highlighting the application's icon in the operating system taskbar.
        void RequestUserAttention() const;

        /// @brief Get the current monitor this window is on
        /// @return The window's current monitor
        const Monitor& GetCurrentMonitor() { return m_CurrentMonitor; }

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

        static const std::vector<Monitor>& GetAvailableMonitors() { return s_Monitors; }

        static void CloseAll();

        static const Monitor& GetPrimaryMonitor();

        static std::vector<const char*> GetVKRequiredInstanceExtensions();

        static bool IsInitialized() { return s_Initialized; }

    private:
        void DetectCurrentMonitor();

        void InitWindowCallbacks();

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

        // Static GLFW callbacks
        // TODO: move these outside of window class, they are part of the platforming backend
        static void InitStaticCallbacks();
        static void GLFWErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int event);

        friend class Application; // for the below functions
        static void Init();
        static void ProcessEvents();
        static void Shutdown();

        static void ProcessMonitors();

    private:
        GLFWwindow* m_GLFWWindow = nullptr;
        std::weak_ptr<Window> m_Handle;
        std::shared_ptr<InputSystem> m_InputSystem;
        std::function<void(Event&)> m_EventCallback;

        Size2D m_Size = WindowConstants::DefaultWindowedSize;
        glm::ivec2 m_Position = {};
        std::string m_Title = WindowConstants::DefaultWindowTitle.data();
        WindowMode m_WindowMode = WindowConstants::DefaultWindowMode;
        bool m_Minimized = false;

        // The size and position of the window when it was in windowed mode
        glm::ivec2 m_LastWindowedPosition = {};
        Size2D m_LastWindowedSize = WindowConstants::DefaultWindowedSize;

        // The current monitor this window is on
        Monitor m_CurrentMonitor = {};

        bool m_ShowOnceRendererIsWorking = WindowConstants::DefaultShowOnceRendererIsWorking;

    private:
        static inline bool s_Initialized = false;

        // Storage of all windows and monitors
        static inline std::vector<std::shared_ptr<Window>> s_Windows;
        static inline std::vector<Monitor> s_Monitors;

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

        inline WindowMode ToWindowMode(std::string_view modeString)
        {
            const std::unordered_map<std::string, WindowMode> stringToWindow = {
                {   Utils::ToString(WindowMode::Windowed),   WindowMode::Windowed },
                { Utils::ToString(WindowMode::Borderless), WindowMode::Borderless },
                { Utils::ToString(WindowMode::Fullscreen), WindowMode::Fullscreen },
            };

            try
            {
                return stringToWindow.at(modeString.data());
            }
            catch (std::out_of_range& e)
            {
                return WindowMode::Windowed;
            }
        }
    }
}