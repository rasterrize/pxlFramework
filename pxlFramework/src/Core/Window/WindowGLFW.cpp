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
                Logger::Log(LogLevel::Info, "Initialized GLFW");
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
            case RendererAPIType::DirectX11:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                break;
            case RendererAPIType::DirectX12:
                Logger::LogError("DirectX12 window support not implemented");
                return;
        }

        m_Window = glfwCreateWindow((int)windowSpecs.Width, (int)windowSpecs.Height, windowSpecs.Title.c_str(), NULL, NULL);
        glfwSetWindowUserPointer(m_Window, (Window*)this);
        SetGLFWCallbacks();

        // Check to see if the window object was created successfully
        if (m_Window)
        {
            Logger::Log(LogLevel::Info, "Successfully created window '" + windowSpecs.Title + "' of size " + std::to_string(windowSpecs.Width) + "x" + std::to_string(windowSpecs.Height));
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
        s_WindowHandles.erase(std::find(s_WindowHandles.begin(), s_WindowHandles.end(), m_Handle));
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

        glfwSetKeyCallback(m_Window, Input::GLFWKeyCallback);
        glfwSetMouseButtonCallback(m_Window, Input::GLFWMouseButtonCallback);
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

    void WindowGLFW::SetSize(unsigned int width, unsigned int height)
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

    void WindowGLFW::SetWindowMode(WindowMode winMode)
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

    void WindowGLFW::SetMonitor(unsigned int monitorIndex)
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
}