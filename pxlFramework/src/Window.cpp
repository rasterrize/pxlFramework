#include "Window.h"

namespace pxl
{
    GLFWwindow* Window::m_Window;

    void Window::Init(unsigned int width, unsigned int height, std::string title)
    {
        glfwInit();

        m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), NULL, NULL);
        
        glfwMakeContextCurrent(m_Window);

        Update();
    }

    void Window::Update()
    {
        while(!glfwWindowShouldClose(m_Window))
        {
            glfwSwapBuffers(m_Window);
            glfwPollEvents();
        }

        glfwTerminate();
    }

    void Window::Shutdown()
    {
        glfwTerminate();
    }
}