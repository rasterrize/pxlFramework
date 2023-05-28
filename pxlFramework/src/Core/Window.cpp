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
        }

        return false;
    }

    void Window::Update()
    {
        glfwPollEvents();
        if (s_GraphicsContext)
            s_GraphicsContext->SwapBuffers();
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
        glfwSetKeyCallback(m_Window, Input::KeyCallback);
        glfwSetWindowSizeCallback(m_Window, WindowResizeCallback);
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

    void Window::SetWindowMode(WindowMode mode)
    {
        switch (mode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(m_Window, NULL, 100, 100, m_WindowSpecs.Width, m_WindowSpecs.Height, GLFW_DONT_CARE);
                SetSize(1280, 720);
                m_WindowMode = WindowMode::Windowed;
                Logger::LogInfo("Set window mode to Windowed");
                break;
            case WindowMode::Borderless:
                m_WindowMode = WindowMode::Borderless;
                //const GLFWvidmode* mode = glfwGetVideoMode(glfwGetWindowMonitor(m_Window));
                Logger::LogInfo("Set window mode to Borderless (not implemented yet)");
                return;
                //break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0, m_WindowSpecs.Width, m_WindowSpecs.Height, GLFW_DONT_CARE);
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
}