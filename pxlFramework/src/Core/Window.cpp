#include "Window.h"
#include "../Renderer/OpenGL/OpenGLContext.h"
#include "Application.h"
#include "Input.h"
#include <glad/glad.h>
#include "../Debug/ImGui/ImGuiOpenGL.h"

namespace pxl
{
    GLFWwindow* Window::s_Window;
    unsigned int Window::s_Width;
    unsigned int Window::s_Height;
    WindowMode Window::s_WindowMode;

    void Window::Init(unsigned int width, unsigned int height, std::string title, RendererAPIType rendererAPI)
    {
        if (s_Window)
        {
            Logger::Log(LogLevel::Warn, "Can't initialize window, one already exists!");
            return;
        }

        // Create window based on graphics api selected
        switch (rendererAPI)
        {
            case RendererAPIType::OpenGL:
                InitGLFWWindow(width, height, title);
            break;
            case RendererAPIType::Vulkan:
                InitGLFWWindow(width, height, title);
            break;
            case RendererAPIType::DirectX12:
                Logger::LogError("Can't initialize window for DX12");
                Application::Get().Close();
                return;
            break;
        }

        s_Width = width;
        s_Height = height;

        Renderer::Init(rendererAPI);

        // Set window callbacks
        SetCallbacks();
    }

    void Window::Update()
    {
        if (s_Window)
        {
            glfwPollEvents();
        }
    }

    void Window::Shutdown()
    {
        //if (Renderer::)
        glfwDestroyWindow(s_Window);
        glfwTerminate();
    }

    void Window::InitGLFWWindow(unsigned int width, unsigned int height, std::string title) // refresh rate/other params
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

        s_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), NULL, NULL);
        s_WindowMode = WindowMode::Windowed;

        // Check to see if the window object was created successfully
        if (s_Window)
        {
            Logger::Log(LogLevel::Info, "Successfully created window '" + title + "' of size " + std::to_string(width) + "x" + std::to_string(height));
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to create window '" + title + "'");
            return;
        }
    }

    void Window::SetCallbacks()
    {
        glfwSetWindowCloseCallback(s_Window, WindowCloseCallback);
        glfwSetKeyCallback(s_Window, Input::KeyCallback);
        glfwSetWindowSizeCallback(s_Window, WindowResizeCallback);
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        Application::Get().Close();
    }

    void Window::WindowResizeCallback(GLFWwindow *window, int width, int height)
    {
        s_Width = width;
        s_Height = height;
    }

    void Window::SetSize(unsigned int width, unsigned int height)
    {
        glfwSetWindowSize(s_Window, width, height);

        // Check for successful window size change
        int windowWidth, windowHeight;
        glfwGetWindowSize(s_Window, &windowWidth, &windowHeight);
        if (windowWidth != width | windowHeight != height)
        {
            Logger::LogWarn("Failed to change window resolution to " + std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
            return;
        }

        glViewport(0, 0, width, height); // GLFW NOTE: Do not pass the window size to glViewport or other pixel-based OpenGL calls. (will fix later)
    }

    void Window::SetWindowMode(WindowMode mode)
    {
        switch (mode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(s_Window, NULL, 500, 500, s_Width, s_Height, GLFW_DONT_CARE);
                SetSize(1280, 720);
                s_WindowMode = WindowMode::Windowed;
                Logger::LogInfo("Set window mode to Windowed");
            break;
            case WindowMode::Borderless:
                s_WindowMode = WindowMode::Borderless;
                //const GLFWvidmode* mode = glfwGetVideoMode(glfwGetWindowMonitor(s_Window));
                Logger::LogInfo("Set window mode to Borderless (not implemented yet)");
                return;
            break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(s_Window, glfwGetPrimaryMonitor(), 0, 0, s_Width, s_Height, GLFW_DONT_CARE);
                s_WindowMode = WindowMode::Fullscreen;
                Logger::LogInfo("Set window mode to Fullscreen");
            break;
        }
    }

    void Window::NextWindowMode()
    {
        switch(s_WindowMode)
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