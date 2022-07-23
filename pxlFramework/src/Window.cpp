#include "Window.h"
#include <glad/glad.h>

namespace pxl
{
    GLFWwindow* Window::m_Window;

    void Window::Init(unsigned int width, unsigned int height, std::string title)
    {
        glfwInit();

        m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), NULL, NULL);
        
        glfwMakeContextCurrent(m_Window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    void Window::Update()
    {
        std::cout << "Window Update" << std::endl;
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    void Window::Shutdown()
    {
        glfwTerminate();
    }
}