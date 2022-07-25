#include "Window.h"
#include <glad/glad.h>

namespace pxl
{
    GLFWwindow* Window::s_Window;

    void Window::Init(unsigned int width, unsigned int height, std::string title)
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
        }

        s_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), NULL, NULL);

        if (s_Window)
        {
            Logger::Log(LogLevel::Info, "Successfully created window '" + title + "' of size " + std::to_string(width) + "x" + std::to_string(height));
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to create window '" + title + "'");
        }

        glfwMakeContextCurrent(s_Window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    void Window::Update()
    {
        if (s_Window)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(s_Window);
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

    }
}