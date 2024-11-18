#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <glm/vec2.hpp>

#include "Core/Image.h"
#include "Core/Size.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/RendererAPIType.h"

namespace pxl
{
    static constexpr Size2D k_DefaultWindowSize = { 640, 480 };
    static constexpr std::string_view k_DefaultWindowTitle = "Default Window Title";

    enum class WindowMode
    {
        Windowed,
        Borderless,
        Fullscreen
    };

    struct Monitor
    {
        uint8_t Index = 1; // refers to the operating system's ID for the monitor
        std::string Name;
        std::vector<GLFWvidmode> VideoModes;
        bool IsPrimary = false;
        GLFWmonitor* GLFWMonitor = nullptr;

        const GLFWvidmode* GetCurrentVideoMode() const { return glfwGetVideoMode(GLFWMonitor); }
    };

    struct WindowSpecs
    {
        std::string Title = std::string(k_DefaultWindowTitle);
        Size2D Size = k_DefaultWindowSize;
        glm::ivec2 Position = { 0.0f, 0.0f };
        WindowMode WindowMode = WindowMode::Windowed;
        RendererAPIType RendererAPI = RendererAPIType::None;
        std::optional<std::string> IconPath;
        std::optional<Monitor> Monitor;
    };

    class Window
    {
    public:
        void Close() const;

        void SetSize(uint32_t width, uint32_t height);

        // To specify only a minimum/maximum, use -1 for width AND height of the opposite pair
        void SetSizeLimits(uint32_t minWidth, uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight);

        void SetPosition(int32_t x, int32_t y);

        void SetWindowMode(WindowMode winMode);

        void SetMonitor(uint8_t index); // OS monitor index
        void SetMonitor(const Monitor& monitor);

        GLFWwindow* GetNativeWindow() const { return m_GLFWWindow; }

        std::shared_ptr<GraphicsContext> GetGraphicsContext() const { return m_GraphicsContext; }

        Size2D GetSize() const { return m_Size; }
        uint32_t GetWidth() const { return m_Size.Width; }
        uint32_t GetHeight() const { return m_Size.Height; }

        WindowMode GetWindowMode() const { return m_WindowMode; }

        RendererAPIType GetRendererAPI() const { return m_RendererAPI; }

        float GetAspectRatio() const { return m_AspectRatio; }

        Size2D GetFramebufferSize() const;

        void NextWindowMode();
        void ToggleFullscreen();

        void Minimize();
        void Maximize();
        void Restore();

        bool GetVisibility() const { return glfwGetWindowAttrib(m_GLFWWindow, GLFW_VISIBLE); }
        void SetVisibility(bool value);

        const std::string& GetTitle() const { return m_Title; }
        void SetTitle(const std::string_view& title) const { glfwSetWindowTitle(m_GLFWWindow, title.data()); }

        void SetIcon(const std::shared_ptr<Image>& image);

        // Use -1 for both parameters to disable
        void EnforceAspectRatio(uint32_t numerator, uint32_t denominator);

        // Highlights the window in the taskbar to grab the user's attention for something
        void RequestUserAttention();

        void SetResizeCallback(const std::function<void(Size2D newSize)>& callback) { m_UserResizeCallback = callback; }
        void SetFileDropCallback(const std::function<void(std::vector<std::string>)>& callback) { m_UserFileDropCallback = callback; }

        static const std::vector<Monitor>& GetMonitors()
        {
            UpdateMonitors();
            return s_Monitors;
        }

        static const Monitor& GetPrimaryMonitor();

        static std::vector<const char*> GetVKRequiredInstanceExtensions();

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);

        static void CloseAll();

    private:
        Window(const WindowSpecs& specs);

        void Update() const;

        void UpdateAspectRatio() { m_AspectRatio = static_cast<float>(m_Size.Width) / static_cast<float>(m_Size.Height); }

        // TODO: Change this to retrieve custom Monitor object
        GLFWmonitor* GetPositionRelativeGLFWMonitor();

        void SetGLFWCallbacks();
        static void SetStaticGLFWCallbacks();

        // Per-window GLFW callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
        static void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos);
        static void WindowDropCallback(GLFWwindow* window, int count, const char** paths);

        // Static GLFW callbacks
        static void GLFWErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int event);

        friend class Application; // for below functions
        static void Init();
        static void ProcessEvents() { s_EventProcessFunc(); }
        static void UpdateAll();
        static void Shutdown();

        static void UpdateMonitors();

    private:
        GLFWwindow* m_GLFWWindow = nullptr;
        std::shared_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        std::weak_ptr<Window> m_Handle;

        Size2D m_Size = k_DefaultWindowSize;
        glm::ivec2 m_Position = { 0, 0 };
        std::string m_Title = std::string(k_DefaultWindowTitle);
        WindowMode m_WindowMode = WindowMode::Windowed;
        RendererAPIType m_RendererAPI = RendererAPIType::None;
        bool m_Minimized = false;
        float m_AspectRatio = 16.0f / 9.0f;

        // TODO:
        Monitor m_CurrentMonitor;

        // User callbacks
        std::function<void(Size2D)> m_UserResizeCallback;
        std::function<void(const std::vector<std::string>&)> m_UserFileDropCallback = nullptr;

        Size2D m_LastWindowedSize = k_DefaultWindowSize; // TODO: make these change when the window size changes via user resize | also I think these might be unnecessary since GLFW stores the previous window size

        static inline std::vector<std::shared_ptr<Window>> s_Windows;
        static inline std::vector<Monitor> s_Monitors;

        static inline std::function<void()> s_EventProcessFunc = glfwPollEvents;
    };
}