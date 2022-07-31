#include "Application.h"
#include "Window.h"
#include "Input.h"

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
            OnUpdate();
        }
    }

    void Application::Close()
    {
        Window::Shutdown();
        Input::Shutdown();  
    }
}