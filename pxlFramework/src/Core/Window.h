#pragma once

#include <GLFW/glfw3.h>
#include "../Renderer/Renderer.h"

namespace pxl
{
    enum class WindowMode
    {
        Windowed,
        Borderless,
        Fullscreen
    };

    struct WindowSpecs
    {
        unsigned int Width;
        unsigned int Height;
        std::string Title;
        RendererAPIType RendererAPI;
    };

    class Window
    {
    public:
        Window(const WindowSpecs& windowSpecs);

        unsigned int GetWidth() { return m_WindowSpecs.Width; }
        unsigned int GetHeight() { return m_WindowSpecs.Height; }

        float GetAspectRatio() { return ((float)m_WindowSpecs.Width / m_WindowSpecs.Height); } // should be cached in a variable

        void SetSize(unsigned int width, unsigned int height);

        void SetWindowMode(WindowMode winMode);
        void NextWindowMode();
        void ToggleFullscreen();
        void SetVsync(bool vsync);
        void ToggleVsync();

        void SetMonitor(unsigned int monitorIndex);

        GLFWwindow* GetNativeWindow() { return m_Window; }

        void Close();
        static void Shutdown();

        static const float GetFPS() { return s_FPS; }
        static const float GetFrameTimeMS() { return 1 / s_FPS * 1000; }

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs) { return std::make_shared<Window>(windowSpecs); }
    private:
        friend class Application;
        static void Update();

        void Init(const WindowSpecs& windowSpecs);

        bool InitGLFWWindow(const WindowSpecs& windowSpecs);
        void SetGLFWCallbacks();

        static void CalculateFPS();

        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
    private:
        GLFWwindow* m_Window;
        static std::unique_ptr<GraphicsContext> s_GraphicsContext;

        WindowSpecs m_WindowSpecs;
        WindowMode m_WindowMode;

        static bool s_Vsync;
        static unsigned int s_FPSLimit;
        static bool s_Minimized;

        static uint16_t s_FrameCount;
        static double s_ElapsedTime; // could be in application class
        static float s_FPS;

        static uint8_t s_WindowCount;

        static GLFWmonitor** s_Monitors;
        static int s_MonitorCount;
    };
}