#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Platform.h"
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
            float time = (float)Platform::GetTime();
            float timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                Camera::Update();
                OnUpdate(timestep);
                pxl_ImGui::Update();
            }

            Window::Update();
        }
    }

    void Application::Close()
    {
        Window::Shutdown(); // could these be put into the test app destructor to reduce overhead?
        Input::Shutdown();  
        pxl_ImGui::Shutdown();
        m_Running = false;
        s_Instance = nullptr;
    }
}