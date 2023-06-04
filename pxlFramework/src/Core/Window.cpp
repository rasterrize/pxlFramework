#include "Window.h"
#include "../Renderer/OpenGL/OpenGLContext.h"
#include "Application.h"
#include "Input.h"
#include <glad/glad.h>
#include "../Debug/ImGui/ImGuiOpenGL.h"

namespace pxl
{
    uint8_t Window::s_WindowCount = 0;

    std::unique_ptr<GraphicsContext> Window::s_GraphicsContext;

    GLFWmonitor** Window::s_Monitors;
    int Window::s_MonitorCount;

    uint16_t Window::s_FrameCount;
    double Window::s_ElapsedTime;
    float Window::s_FPS;
    bool Window::s_Vsync = true;
    unsigned int Window::s_FPSLimit = 0;
    bool Window::s_Minimized = false;

    Window::Window(const WindowSpecs& windowSpecs)
        : m_WindowSpecs(windowSpecs), m_WindowMode(WindowMode::Windowed)
    {
        Init(windowSpecs);
    }

    void Window::Init(const WindowSpecs& windowSpecs)
    {
        if (s_WindowCount > 0)
        {
            Logger::LogError("Can't initialize window, only 1 can exist atm");
            return;
        }

        // Create window based on graphics api selected
        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                Logger::LogInfo("RendererAPI type 'none' specified! Creating a window with no renderer api...");
                InitGLFWWindow(windowSpecs);
                break;
            case RendererAPIType::OpenGL:
                InitGLFWWindow(windowSpecs);

                if (!s_GraphicsContext)
                    s_GraphicsContext = std::make_unique<OpenGLContext>(m_Window);

                if (!Renderer::IsInitialized())
                    Renderer::Init(RendererAPIType::OpenGL);

                break;
            case RendererAPIType::Vulkan:
                InitGLFWWindow(windowSpecs);
                break;
            case RendererAPIType::DirectX11:
                //InitWin32Window(width, height, title);
                Logger::LogError("Can't initialize window for DX11");
                return;
            case RendererAPIType::DirectX12:
                Logger::LogError("Can't initialize window for DX12");
                return;
        }
        s_WindowCount++;
    }

    bool Window::InitGLFWWindow(const WindowSpecs& windowSpecs) // refresh rate/other params
    {
        if (glfwInit())
        {
            Logger::Log(LogLevel::Info, "Initialized GLFW");
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to initialize GLFW");
            return false;
        }

        // reset window hints for less confusion
        glfwDefaultWindowHints();

        // Set window hints based on renderer api
        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            case RendererAPIType::OpenGL:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                break;
            case RendererAPIType::Vulkan:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                break;
            case RendererAPIType::DirectX11:
                Logger::LogError("GLFW doesn't support DirectX 11!");
                return false;
            case RendererAPIType::DirectX12:
                Logger::LogError("GLFW doesn't support DirectX 12!");
                return false;
        }

        m_Window = glfwCreateWindow((int)windowSpecs.Width, (int)windowSpecs.Height, windowSpecs.Title.c_str(), NULL, NULL);
        glfwSetWindowUserPointer(m_Window, this);
        glfwSwapInterval(s_Vsync);
        SetGLFWCallbacks();

        // Check to see if the window object was created successfully
        if (m_Window)
        {
            Logger::Log(LogLevel::Info, "Successfully created window '" + windowSpecs.Title + "' of size " + std::to_string(windowSpecs.Width) + "x" + std::to_string(windowSpecs.Height));
            return true;
        }
        else 
        {
            Logger::Log(LogLevel::Error, "Failed to create window '" + windowSpecs.Title + "'");
            if (s_WindowCount == 0)
                glfwTerminate();
        }

        return false;
    }

    void Window::Update()
    {
        // glfw docs show poll events after swap buffers
        if (s_GraphicsContext)
            s_GraphicsContext->SwapBuffers();

        if (!s_Minimized)
            glfwPollEvents();
        else
            glfwWaitEvents(); // this could be bad if a game needs updating in the background
        
        s_FrameCount++;
        CalculateFPS();
    }

    void Window::Close()
    {
        glfwDestroyWindow(m_Window);
        --s_WindowCount;

        if (s_WindowCount == 0)
            s_GraphicsContext = nullptr;
            Application::Get().Close();
    }

    void Window::Shutdown()
    {
        glfwTerminate();
    }

    void Window::SetGLFWCallbacks()
    {
        glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_Window, WindowResizeCallback);
        glfwSetWindowIconifyCallback(m_Window, WindowIconifyCallback);

        glfwSetKeyCallback(m_Window, Input::KeyCallback);
        glfwSetMouseButtonCallback(m_Window, Input::MouseButtonCallback);
        glfwSetCursorPosCallback(m_Window, Input::CursorPosCallback);
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        auto windowHandle = (Window*)glfwGetWindowUserPointer(window);
        windowHandle->Close();
    }

    void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto windowHandle = (Window*)glfwGetWindowUserPointer(window);
        windowHandle->m_WindowSpecs.Width = width;
        windowHandle->m_WindowSpecs.Height = height;

        glViewport(0, 0, width, height); // GLFW NOTE: Do not pass the window size to glViewport or other pixel-based OpenGL calls. (will fix later)
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        // probably won't work for windows. Should look into what iconification really is
        if (iconified)
        {
            s_Minimized = true;
            Application::Get().SetMinimization(true);
        }
        else
        {
            s_Minimized = false;
            Application::Get().SetMinimization(false);
        }
    }

    void Window::SetSize(unsigned int width, unsigned int height)
    {
        glfwSetWindowSize(m_Window, width, height);

        // Check for successful window size change
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);
        if (windowWidth != width | windowHeight != height)
        {
            Logger::LogWarn("Failed to change window resolution to " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
            return;
        }
    }

    void Window::SetWindowMode(WindowMode winMode)
    {

        const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int vidmodeCount;
        const GLFWvidmode* vidmodes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &vidmodeCount);

        switch (winMode)
        {
            case WindowMode::Windowed:
                // Should set it to what monitor its on
                glfwSetWindowMonitor(m_Window, NULL, 100, 100, m_WindowSpecs.Width, m_WindowSpecs.Height, GLFW_DONT_CARE);
                SetSize(1280, 720);
                m_WindowMode = WindowMode::Windowed;
                Logger::LogInfo("Set window mode to Windowed");
                break;
            case WindowMode::Borderless:

                // Should set it to what monitor its on
                //auto monitor = glfwGetWindowMonitor(m_Window);
                
                glfwSetWindowMonitor(m_Window, NULL, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);

                m_WindowMode = WindowMode::Borderless;
                Logger::LogInfo("Set window mode to Borderless");
                break;
            case WindowMode::Fullscreen:
                // Should set it to what monitor its on
                glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                m_WindowMode = WindowMode::Fullscreen;
                Logger::LogInfo("Set window mode to Fullscreen");
                break;
        }
    }

    void Window::NextWindowMode()
    {
        switch(m_WindowMode)
        {
            case WindowMode::Windowed:
                SetWindowMode(WindowMode::Borderless);
                break;
            case WindowMode::Borderless:
                SetWindowMode(WindowMode::Fullscreen);
                break;
            case WindowMode::Fullscreen:
                SetWindowMode(WindowMode::Windowed);
                break;
        }
    }

    void Window::ToggleFullscreen()
    {
        if (m_WindowMode == WindowMode::Windowed || m_WindowMode == WindowMode::Borderless)
            SetWindowMode(WindowMode::Fullscreen);
        else if (m_WindowMode == WindowMode::Fullscreen)
            SetWindowMode(WindowMode::Windowed);
    }

    void Window::SetVsync(bool vsync)
    {
        if (vsync)
        {
            glfwSwapInterval(1);
            s_Vsync = true;
        }
        else
        {
            glfwSwapInterval(0);
            s_Vsync = false;
        }
    }

    void Window::ToggleVsync()
    {
        if (s_Vsync)
            SetVsync(true);
        else
            SetVsync(false);
    }

    void Window::SetMonitor(unsigned int monitorIndex)
    {
        if (monitorIndex == 0)
            return;
        
        if (s_WindowCount == 0)
            return;

        s_Monitors = glfwGetMonitors(&s_MonitorCount);

        auto currentMonitor = glfwGetWindowMonitor(m_Window);
        
        if (monitorIndex > s_MonitorCount)
        {
            Logger::LogWarn("Can't set specified monitor for window '" + m_WindowSpecs.Title + "'. Monitor doesn't exist");
            return;
        }

        GLFWmonitor* monitor = s_Monitors[monitorIndex - 1];
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        int nextXpos, nextYpos;
        int nextWidth, nextHeight;
        glfwGetMonitorWorkarea(monitor, &nextXpos, &nextYpos, &nextWidth, &nextHeight);

        switch (m_WindowMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(m_Window, NULL, nextXpos + 50, nextYpos + 50, m_WindowSpecs.Width, m_WindowSpecs.Height, GLFW_DONT_CARE);
                break;
            case WindowMode::Borderless:
                glfwSetWindowMonitor(m_Window, NULL, nextXpos, nextYpos, mode->width, mode->height, mode->refreshRate);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                break;
        }
    }

    void Window::CalculateFPS()
    {
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - s_ElapsedTime;

        if (elapsedTime > 0.05)
        {
            s_FPS = s_FrameCount / elapsedTime;
            s_ElapsedTime = currentTime;
            s_FrameCount = 0;
        }
    }
}