#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "../Renderer/Camera.h"
#include "../Debug/ImGui/ImGuiOpenGL.h"

namespace pxl
{
    Application* Application::s_Instance;

    Application::Application()
    {
        if (s_Instance)
        {
            Logger::Log(LogLevel::Error, "Can't create application, one already exists");
            throw;
        }
        s_Instance = this;
    }

    void Application::Run()
    {
        while (m_Running)
        {
            // these should maybe check if they are initalized first
            Camera::Update();
            OnUpdate();
            //pxl_ImGui::Update();
            Window::Update(); // Does this go before or after?
        }
    }

    void Application::Close()
    {
        Window::Shutdown();
        Input::Shutdown();  
        s_Instance = nullptr;
        m_Running = false;
    }
}