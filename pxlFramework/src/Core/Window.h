#pragma once

#include "WindowMode.h"
#include "../Renderer/GraphicsContext.h"
#include "../Renderer/RendererAPIType.h"

#include <GLFW/glfw3.h>

namespace pxl
{
    struct WindowSpecs
    {
        uint32_t Width = 640;
        uint32_t Height = 480;
        std::string Title = "";
        RendererAPIType RendererAPI = RendererAPIType::None;
        WindowMode WindowMode = WindowMode::Windowed;
        bool Minimized = false;
    };

    class Window
    {
    public:
        Window(const WindowSpecs& windowSpecs);

        void Update();
        void Close();

        void CreateGLFWWindow(const WindowSpecs& windowSpecs);

        void SetSize(uint32_t width, uint32_t height);
        void SetPosition(uint32_t x, uint32_t y);
        void SetWindowMode(WindowMode winMode);
        void SetMonitor(uint8_t monitorIndex);

        GLFWwindow* GetNativeWindow() const { return m_GLFWWindow; }

        std::shared_ptr<GraphicsContext> GetGraphicsContext() const { return m_GraphicsContext; }

        WindowSpecs GetWindowSpecs() const { return m_WindowSpecs; }

        uint32_t GetWidth() const { return m_WindowSpecs.Width; }
        uint32_t GetHeight() const { return m_WindowSpecs.Height; }
        WindowMode GetWindowMode() const { return m_WindowSpecs.WindowMode; }

        float GetAspectRatio() const { return ((float)m_WindowSpecs.Width / m_WindowSpecs.Height); } // should be cached in a variable

        void NextWindowMode();
        void ToggleFullscreen();

        void SetVSync(bool vsync) { m_GraphicsContext->SetVSync(vsync); }
        void ToggleVSync();

        void SetGLFWCallbacks();
        GLFWmonitor* GetCurrentMonitor();
    public:
        static void Shutdown(); // I don't think this should be public
        static int GetMonitorCount() { return s_MonitorCount; }

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);

        // GLFW callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
        static void MonitorCallback(GLFWmonitor* monitor, int event);
    private:
        friend class Application; // for UpdateAll()
        static void UpdateAll();

        static void ProcessEvents();

        static void GetGLFWMonitors();
    private:
        GLFWwindow* m_GLFWWindow;
        std::shared_ptr<GraphicsContext> m_GraphicsContext;
        std::shared_ptr<Window> m_Handle;

        WindowSpecs m_WindowSpecs = {};

        uint32_t m_LastWindowedWidth = m_WindowSpecs.Width; // TODO: make these change when the window size changes via user resize
        uint32_t m_LastWindowedHeight = m_WindowSpecs.Height;

        static uint8_t s_WindowCount;
        static uint8_t s_MonitorCount;

        static std::vector<std::shared_ptr<Window>> s_Windows;

        static uint8_t s_GLFWWindowCount;
        static GLFWmonitor** s_Monitors;
    };
}