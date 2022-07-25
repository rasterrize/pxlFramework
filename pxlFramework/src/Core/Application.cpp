#include "Application.h"
#include "Window.h"

namespace pxl
{
    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        while (m_Running)
        {
            Window::Update();
        }
    }

    void Application::Close()
    {
        Window::Shutdown();
        //Input::Shutdown();  
    }
}