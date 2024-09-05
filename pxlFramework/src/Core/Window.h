#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp> // decided to use glm here since its a good general math library as well
#include <vulkan/vulkan.h>

#include "Renderer/GraphicsContext.h"
#include "Renderer/RendererAPIType.h"

namespace pxl
{
    static constexpr uint32_t DEFAULT_WINDOW_WIDTH = 640;
    static constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 480;

    enum class WindowMode
    {
        Windowed = 0,
        Borderless,
        Fullscreen
    };

    struct WindowSpecs
    {
        uint32_t Width = DEFAULT_WINDOW_WIDTH;
        uint32_t Height = DEFAULT_WINDOW_HEIGHT;
        std::string Title;
        WindowMode WindowMode = WindowMode::Windowed;
        RendererAPIType RendererAPI = RendererAPIType::None;
        bool Minimized = false;
    };

    struct Monitor
    {
        std::string Name;
        uint8_t Index = 0; // refers to the operating system's ID for the monitor, so 0 is should be invalid
        GLFWmonitor* GLFWMonitor = nullptr;
        std::vector<GLFWvidmode> VideoModes;
        bool IsPrimary = false;
        //int BitDepth;

        const GLFWvidmode* GetCurrentVideoMode() const { return glfwGetVideoMode(GLFWMonitor); }
    };

    class Window
    {
    public:
        Window(const WindowSpecs& windowSpecs);

        void Close() const;

        void SetSize(uint32_t width, uint32_t height);

        void SetPosition(uint32_t x, uint32_t y);

        void SetWindowMode(WindowMode winMode);

        void SetMonitor(uint8_t index); // OS monitor index
        void SetMonitor(const Monitor& monitor);

        GLFWwindow* GetNativeWindow() const { return m_GLFWWindow; }

        std::shared_ptr<GraphicsContext> GetGraphicsContext() const { return m_GraphicsContext; }

        const WindowSpecs& GetWindowSpecs() const { return m_Specs; }

        glm::u32vec2 GetSize() { return { m_Specs.Width, m_Specs.Height }; }

        uint32_t GetWidth() const { return m_Specs.Width; }

        uint32_t GetHeight() const { return m_Specs.Height; }

        WindowMode GetWindowMode() const { return m_Specs.WindowMode; }

        float GetAspectRatio() const { return static_cast<float>(m_Specs.Width) / static_cast<float>(m_Specs.Height); } // should this be cached in a variable? | could be updated every window resize callback
        
        glm::u32vec2 GetFramebufferSize() const;

        VkSurfaceKHR CreateVKWindowSurface(VkInstance instance); // Keeping vulkan here for now but obviously not ideal because it shouldnt be tied to window class

        void NextWindowMode();
        void ToggleFullscreen();

        void SetVSync(bool vsync) const { m_GraphicsContext->SetVSync(vsync); }
        void ToggleVSync() { SetVSync(!m_GraphicsContext->GetVSync()); }

        bool GetVisibility() const { return glfwGetWindowAttrib(m_GLFWWindow, GLFW_VISIBLE); }
        void SetVisibility(bool value);

        void SetTitle(const std::string_view& title) const { glfwSetWindowTitle(m_GLFWWindow, title.data()); }

        GLFWmonitor* GetWindowsCurrentGLFWMonitor();

        static const std::vector<Monitor>& GetMonitors() { return s_Monitors; }

        static const Monitor& GetPrimaryMonitor() { for (const auto& monitor : s_Monitors) { if (monitor.IsPrimary) return monitor; } return s_Monitors[0]; }

        static std::vector<const char*> GetVKRequiredInstanceExtensions();
        
        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);
    private:
        void CreateGLFWWindow(const WindowSpecs& windowSpecs);
        void Update() const;

        void SetGLFWCallbacks();
        static void SetStaticGLFWCallbacks();
        
        // Per-window GLFW callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);

        // Static GLFW callbacks
        static void GLFWErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int event);

    private:
        friend class Application; // for UpdateAll()
        static void UpdateAll();

        static void Shutdown();

        static void UpdateMonitors();
    private:
        GLFWwindow* m_GLFWWindow = nullptr;
        std::shared_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        std::weak_ptr<Window> m_Handle;

        WindowSpecs m_Specs = {};

        uint32_t m_LastWindowedWidth = DEFAULT_WINDOW_WIDTH; // TODO: make these change when the window size changes via user resize | also I think these might be unnecessary since GLFW stores the previous window size
        uint32_t m_LastWindowedHeight = DEFAULT_WINDOW_HEIGHT;

        static inline std::vector<std::shared_ptr<Window>> s_Windows;
        static inline std::vector<Monitor> s_Monitors;

        static inline std::function<void()> s_EventProcessFunc = glfwPollEvents;
    };
}