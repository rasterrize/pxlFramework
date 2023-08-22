#include "WindowGLFW.h"

#include <glad/glad.h>

#include "../../Renderer/Renderer.h"
#include "../../Renderer/GraphicsContext.h"
#include "../Application.h"
#include "../Input.h"

namespace pxl
{
    uint8_t WindowGLFW::s_GLFWWindowCount = 0;

    GLFWmonitor** WindowGLFW::s_Monitors;
    int WindowGLFW::s_MonitorCount;

    WindowGLFW::WindowGLFW(const WindowSpecs& windowSpecs)
        : Window(windowSpecs)
    {
        CreateGLFWWindow(windowSpecs);
        s_WindowCount++;

        if (s_GLFWWindowCount == 1)
            m_GraphicsContext = GraphicsContext::Create(windowSpecs.RendererAPI, m_Window);
    }

    void WindowGLFW::CreateGLFWWindow(const WindowSpecs& windowSpecs) // refresh rate/other params
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

        m_Window = glfwCreateWindow((int)windowSpecs.Width, (int)windowSpecs.Height, windowSpecs.Title.c_str(), NULL, NULL);
        s_Monitors = glfwGetMonitors(&s_MonitorCount);
        glfwSetWindowUserPointer(m_Window, (Window*)this);
        SetGLFWCallbacks();

        // Check to see if the window object was created successfully
        if (m_Window)
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

    void WindowGLFW::Update()
    {
        if (m_GraphicsContext)
            m_GraphicsContext->SwapBuffers();

        Renderer::s_FrameCount++; // should be done at the end of making a frame in the renderer class (Renderer::EndFrame)
        Renderer::CalculateFPS();
    }

    void WindowGLFW::ProcessEvents()
    {
        glfwPollEvents();
    }

    void WindowGLFW::Close()
    {
        glfwDestroyWindow(m_Window);
        s_Windows.erase(std::find(s_Windows.begin(), s_Windows.end(), m_Handle));
        --s_WindowCount;

        if (s_WindowCount == 0)
        {
            glfwTerminate();
            Application::Get().Close();
        }
    }

    void WindowGLFW::SetGLFWCallbacks()
    {
        glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_Window, WindowResizeCallback);
        glfwSetWindowIconifyCallback(m_Window, WindowIconifyCallback);

        glfwSetMonitorCallback(MonitorCallback);

        glfwSetKeyCallback(m_Window, Input::GLFWKeyCallback);
        glfwSetMouseButtonCallback(m_Window, Input::GLFWMouseButtonCallback);
        glfwSetScrollCallback(m_Window, Input::GLFWScrollCallback);
        glfwSetCursorPosCallback(m_Window, Input::GLFWCursorPosCallback);

    }

    void WindowGLFW::WindowCloseCallback(GLFWwindow* window)
    {
        auto windowHandle = (WindowGLFW*)glfwGetWindowUserPointer(window);
        windowHandle->Close();
    }

    void WindowGLFW::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, height); // GLFW NOTE: Do not pass the window size to glViewport or other pixel-based OpenGL calls. (will fix later)
        // ^ this should be in the renderer somewhere.

        auto windowInstance = (WindowGLFW*)glfwGetWindowUserPointer(window);
        windowInstance->m_WindowSpecs.Width = width;
        windowInstance->m_WindowSpecs.Height = height;
    }

    void WindowGLFW::WindowIconifyCallback(GLFWwindow* window, int iconified)
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

    void WindowGLFW::MonitorCallback(GLFWmonitor* monitor, int event)
    {
        if (event == GLFW_CONNECTED || event == GLFW_DISCONNECTED)
        {
            s_Monitors = glfwGetMonitors(&s_MonitorCount);
        }
    }

    GLFWmonitor* WindowGLFW::GetCurrentMonitor()
    {
        int windowX, windowY;
        glfwGetWindowPos(m_Window, &windowX, &windowY);

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

    void WindowGLFW::SetSize(uint32_t width, uint32_t height)
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

    void WindowGLFW::SetPosition(uint32_t xpos, uint32_t ypos)
    {
        glfwSetWindowPos(m_Window, xpos, ypos); // TODO: should this take the monitor into consideration?
        // TODO: add logging
    }

    void WindowGLFW::SetWindowMode(WindowMode winMode)
    { 
        if (winMode == m_WindowMode)
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
                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, GLFW_TRUE);
                glfwSetWindowMonitor(m_Window, nullptr, monitorX + (vidmode->width / 2) - (m_LastWindowedWidth / 2), monitorY + (vidmode->height / 2) - (m_LastWindowedHeight / 2), m_LastWindowedWidth, m_LastWindowedHeight, GLFW_DONT_CARE); // TODO: store the windowed window size so it can be restored instead of fixed 1280x720
                m_WindowMode = WindowMode::Windowed;
                Logger::LogInfo("Switched window mode to Windowed");
                break;
            case WindowMode::Borderless:
                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, GLFW_FALSE);
                glfwSetWindowMonitor(m_Window, nullptr, monitorX, monitorY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                m_WindowMode = WindowMode::Borderless;
                Logger::LogInfo("Switched window mode to Borderless");
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_Window, currentMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                m_GraphicsContext->SetVSync(m_GraphicsContext->GetVSync()); // Set VSync because bug idk
                m_WindowMode = WindowMode::Fullscreen;
                Logger::LogInfo("Switched window mode to Fullscreen");
                break;
        }
    }

    void WindowGLFW::SetMonitor(uint8_t monitorIndex)
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

        switch (m_WindowMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(m_Window, nullptr, nextMonX + (vidmode->width / 2) - (m_WindowSpecs.Width / 2), nextMonY + (vidmode->height / 2) - (m_WindowSpecs.Height / 2), m_WindowSpecs.Width, m_WindowSpecs.Height, GLFW_DONT_CARE);
                break;
            case WindowMode::Borderless:
                glfwSetWindowMonitor(m_Window, nullptr, nextMonX, nextMonY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_Window, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                break;
        }
    }
}