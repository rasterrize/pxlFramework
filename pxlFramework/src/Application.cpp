#include "Application.h"
#include "Window.h"

namespace pxl
{
    Application::Application()
    {
        Window::Init(1280, 720, "pxlFramework");
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
    }
}