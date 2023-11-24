#include "Window.h"

#include "../Renderer/Renderer.h"

#include "Input.h"
#include "../Debug/ImGui/pxl_ImGui.h"

namespace pxl
{
    uint8_t Window::s_WindowCount = 0;
    uint8_t Window::s_MonitorCount = 0;

    uint8_t Window::s_GLFWWindowCount;
    GLFWmonitor** Window::s_Monitors;

    std::vector<std::shared_ptr<Window>> Window::s_Windows;

    Window::Window(const WindowSpecs& windowSpecs)
        : m_WindowSpecs(windowSpecs)
    {
        CreateGLFWWindow(windowSpecs);
        s_WindowCount++;
    }

    void Window::CreateGLFWWindow(const WindowSpecs& windowSpecs) // refresh rate/other params
    {
        if (s_GLFWWindowCount == 0)
        {
            if (glfwInit())
            {
                Logger::Log(LogLevel::Info, "GLFW Initialized");
            }
            else
            {
                Logger::Log(LogLevel::Error, "Failed to initialize GLFW");
                return;
            }
        }

        // reset window hints for stability reasons
        glfwDefaultWindowHints();

        // Set window hints based on renderer api
        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                Logger::LogInfo("RendererAPI type 'none' specified! Creating a GLFW window with no renderer api...");
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            case RendererAPIType::OpenGL:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                break;
            case RendererAPIType::Vulkan:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                break;
        }

        // Create GLFW window and set it up
        m_GLFWWindow = glfwCreateWindow((int)windowSpecs.Width, (int)windowSpecs.Height, windowSpecs.Title.c_str(), NULL, NULL);
        int monitorCount;
        s_Monitors = glfwGetMonitors(&monitorCount);
        s_MonitorCount = (uint32_t)monitorCount;
        glfwSetWindowUserPointer(m_GLFWWindow, (Window*)this);
        SetGLFWCallbacks();

        // Check to see if the window object was created successfully
        if (m_GLFWWindow)
        {
            Logger::Log(LogLevel::Info, "Created window '" + windowSpecs.Title + "' of size " + std::to_string(windowSpecs.Width) + "x" + std::to_string(windowSpecs.Height));
            s_GLFWWindowCount++;
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to create window '" + windowSpecs.Title + "'");
            if (s_WindowCount == 0)
                glfwTerminate();
        }
    }

    void Window::Update()
    {
        if (m_GraphicsContext)
            m_GraphicsContext->Present();
    }

    void Window::Close()
    {
        glfwDestroyWindow(m_GLFWWindow);
        s_Windows.erase(std::find(s_Windows.begin(), s_Windows.end(), m_Handle));
        --s_WindowCount;

        if (pxl_ImGui::GetWindowHandle() == m_Handle)
            pxl_ImGui::Shutdown();

        if (s_WindowCount == 0)
        {
            glfwTerminate();
            Application::Get().Close();
        }
    }

    void Window::SetGLFWCallbacks()
    {
        glfwSetWindowCloseCallback(m_GLFWWindow, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_GLFWWindow, WindowResizeCallback);
        glfwSetWindowIconifyCallback(m_GLFWWindow, WindowIconifyCallback);

        glfwSetMonitorCallback(MonitorCallback);

        glfwSetKeyCallback(m_GLFWWindow, Input::GLFWKeyCallback);
        glfwSetMouseButtonCallback(m_GLFWWindow, Input::GLFWMouseButtonCallback);
        glfwSetScrollCallback(m_GLFWWindow, Input::GLFWScrollCallback);
        glfwSetCursorPosCallback(m_GLFWWindow, Input::GLFWCursorPosCallback);
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        auto windowHandle = (Window*)glfwGetWindowUserPointer(window);
        windowHandle->Close();
    }

    void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, height); // GLFW NOTE: Do not pass the window size to glViewport or other pixel-based OpenGL calls. (will fix later)
        // ^ this should be in the renderer somewhere.

        auto windowInstance = (Window*)glfwGetWindowUserPointer(window);
        windowInstance->m_WindowSpecs.Width = width;
        windowInstance->m_WindowSpecs.Height = height;
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        // probably won't work for windows. Should look into what iconification really is
        if (iconified)
        {
            Application::Get().SetMinimization(true);
        }
        else
        {
            Application::Get().SetMinimization(false);
        }
    }

    void Window::MonitorCallback(GLFWmonitor* monitor, int event)
    {
        if (event == GLFW_CONNECTED || event == GLFW_DISCONNECTED)
        {
            int monitorCount;
            s_Monitors = glfwGetMonitors(&monitorCount);
            s_MonitorCount = (uint8_t)monitorCount;
        }
    }

    GLFWmonitor* Window::GetCurrentMonitor()
    {
        int windowX, windowY;
        glfwGetWindowPos(m_GLFWWindow, &windowX, &windowY);

        int monitorX, monitorY;
        int monitorWidth, monitorHeight;

        for (int i = 0; i < s_MonitorCount; i++)
        {   
            glfwGetMonitorWorkarea(s_Monitors[i], &monitorX, &monitorY, &monitorWidth, &monitorHeight);
            if ((windowX >= monitorX && windowX < (monitorX + monitorWidth)) && (windowY >= monitorY && windowY < (monitorY + monitorHeight)))
            {
                return s_Monitors[i];
            }
        }

        Logger::LogError("Failed to get window current monitor");
        return nullptr;
    }

    void Window::SetSize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(m_GLFWWindow, width, height);

        // Check for successful window size change
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_GLFWWindow, &windowWidth, &windowHeight);
        if (windowWidth != width | windowHeight != height)
        {
            Logger::LogWarn("Failed to change window resolution to " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
            return;
        }
    }

    void Window::SetPosition(uint32_t xpos, uint32_t ypos)
    {
        glfwSetWindowPos(m_GLFWWindow, xpos, ypos); // TODO: should this take the monitor into consideration?
        // TODO: add logging
    }

    void Window::SetWindowMode(WindowMode winMode)
    { 
        if (winMode == m_WindowSpecs.WindowMode)
            return;

        auto currentMonitor = GetCurrentMonitor();
        if (!currentMonitor)
        {
            currentMonitor = glfwGetPrimaryMonitor();
            Logger::LogInfo("Current monitor was null, so the primary monitor was used");
        }

        const GLFWvidmode* vidmode = glfwGetVideoMode(currentMonitor);

        int monitorX, monitorY;
        int monitorWidth, monitorHeight;
        glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

        switch (winMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_TRUE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX + (vidmode->width / 2) - (m_LastWindowedWidth / 2), monitorY + (vidmode->height / 2) - (m_LastWindowedHeight / 2), m_LastWindowedWidth, m_LastWindowedHeight, GLFW_DONT_CARE); // TODO: store the windowed window size so it can be restored instead of fixed 1280x720
                m_WindowSpecs.WindowMode = WindowMode::Windowed;
                Logger::LogInfo("Switched window mode to Windowed");
                break;
            case WindowMode::Borderless:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_FALSE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX, monitorY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                m_WindowSpecs.WindowMode = WindowMode::Borderless;
                Logger::LogInfo("Switched window mode to Borderless");
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, currentMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                m_GraphicsContext->SetVSync(m_GraphicsContext->GetVSync()); // Set VSync because bug idk
                m_WindowSpecs.WindowMode = WindowMode::Fullscreen;
                Logger::LogInfo("Switched window mode to Fullscreen");
                break;
        }
    }

    void Window::SetMonitor(uint8_t monitorIndex)
    {
        if (monitorIndex > s_MonitorCount || monitorIndex <= 0)
        {
            Logger::LogWarn("Can't set specified monitor for window '" + m_WindowSpecs.Title + "'. Monitor doesn't exist");
            return;
        }

        // Get the current video mode of the specified monitor
        GLFWmonitor* monitor = s_Monitors[monitorIndex - 1]; // monitor indexes start at 1, arrays start at 0
        const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

        int nextMonX, nextMonY;
        glfwGetMonitorWorkarea(monitor, &nextMonX, &nextMonY, NULL, NULL);

        switch (m_WindowSpecs.WindowMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, nextMonX + (vidmode->width / 2) - (m_WindowSpecs.Width / 2), nextMonY + (vidmode->height / 2) - (m_WindowSpecs.Height / 2), m_WindowSpecs.Width, m_WindowSpecs.Height, GLFW_DONT_CARE);
                break;
            case WindowMode::Borderless:
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, nextMonX, nextMonY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                break;
        }
    }

    void Window::NextWindowMode()
    {
        switch(m_WindowSpecs.WindowMode)
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
        if (m_WindowSpecs.WindowMode == WindowMode::Windowed || m_WindowSpecs.WindowMode == WindowMode::Borderless)
            SetWindowMode(WindowMode::Fullscreen);
        else if (m_WindowSpecs.WindowMode == WindowMode::Fullscreen)
            SetWindowMode(WindowMode::Windowed);
    }

    void Window::ToggleVSync()
    {
        if (!m_GraphicsContext->GetVSync())
            SetVSync(true);
        else
            SetVSync(false);
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        std::shared_ptr<Window> window;
        window = std::make_shared<Window>(windowSpecs);

        if (window)
        {
            window->m_Handle = window;
            if (windowSpecs.RendererAPI != RendererAPIType::None)
            {
                window->m_GraphicsContext = GraphicsContext::Create(windowSpecs.RendererAPI, window); // Automatically create a graphics context for the window

                if (!window->m_GraphicsContext)
                {
                    Logger::LogError("Failed to create graphics context for window " + windowSpecs.Title);
                }
            }

            s_Windows.push_back(window);
            return window;
        }

        return nullptr;
    }

    void Window::UpdateAll()
    {
        for (const auto& window : s_Windows)
        {
            window->Update();
        }

        Renderer::s_FrameCount++; // should be done at the end of making a frame in the renderer class (Renderer::EndFrame)
        Renderer::CalculateFPS();

        Window::ProcessEvents(); // glfw docs use pollevents after swapbuffers // also this should be moved if I decide to support other platforms (linux/mac)
    }

    void Window::ProcessEvents()
    {
        glfwPollEvents();
    }

    void Window::Shutdown()
    {
        uint32_t windowCount = s_WindowCount;

        for (uint32_t i = 0; i < windowCount; i++)
        {
            s_Windows[0]->Close();
        }
    }
}