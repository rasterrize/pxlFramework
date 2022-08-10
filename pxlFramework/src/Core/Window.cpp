#include "Window.h"
#include "../Renderer/OpenGL/OpenGLContext.h"
#include "Application.h"
#include <glad/glad.h>

namespace pxl
{
    GLFWwindow* Window::s_Window;
    std::unique_ptr<GraphicsContext> Window::s_GraphicsContext;

    void Window::Init(unsigned int width, unsigned int height, std::string title, RendererAPI rendererAPI)
    {   
        if (s_Window)
        {
            Logger::Log(LogLevel::Warn, "Can't initialize window, one already exists!");
            return;
        }

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

        if (s_Window)
        {
            Logger::Log(LogLevel::Info, "Successfully created window '" + title + "' of size " + std::to_string(width) + "x" + std::to_string(height));
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to create window '" + title + "'");
            return;
        }
        
        switch (rendererAPI)
        {
            case RendererAPI::OpenGL:
                s_GraphicsContext = std::make_unique<OpenGLContext>();
                break;        
            case RendererAPI::Vulkan:
                return;
                break;            
            case RendererAPI::DirectX12:
                return;
                break;        
        }

        Renderer::Init(rendererAPI);

        SetCallbacks();
    }

    void Window::Update()
    {
        if (s_Window)
        {
            //Renderer::Clear();
            glClear(GL_COLOR_BUFFER_BIT);
            s_GraphicsContext->SwapBuffers();
            glfwPollEvents();
        }
    }

    void Window::Shutdown()
    {
        glfwDestroyWindow(s_Window);
        glfwTerminate();
    }

    void Window::SetCallbacks()
    {
        glfwSetWindowCloseCallback(s_Window, WindowCloseCallback);
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        Application::Get().Close();
    }
}