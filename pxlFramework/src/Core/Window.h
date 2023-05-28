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

        void SetWindowMode(WindowMode mode);
        void NextWindowMode();
        //void ToggleFullscreen();

        GLFWwindow* GetNativeWindow() { return m_Window; }

        void Close();
        static void Shutdown();

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs) { return std::make_shared<Window>(windowSpecs); }
    private:
        friend class Application;
        static void Update(); // temp, should be private

        void Init(const WindowSpecs& windowSpecs);

        bool InitGLFWWindow(const WindowSpecs& windowSpecs);
        void SetGLFWCallbacks();

        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
    private:
        GLFWwindow* m_Window;
        static std::unique_ptr<GraphicsContext> s_GraphicsContext;

        WindowSpecs m_WindowSpecs;
        WindowMode m_WindowMode;

        static uint8_t s_WindowCount;

        //static bool s_GLFWInitialized;

        //static std::vector<Window*> s_WindowHandles;
    };
}