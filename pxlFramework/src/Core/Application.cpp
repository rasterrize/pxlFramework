#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Platform.h"
#include "../Renderer/Camera.h"
#include "../Debug/ImGui/pxl_ImGui.h"

#include "Stopwatch.h"

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
        Stopwatch stopwatch;
        while (m_Running)
        {
            float time = (float)Platform::GetTime();
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                OnUpdate(deltaTime);
                Camera::Update();
                pxl_ImGui::Update();
            }

            Window::UpdateAll();
        }
        stopwatch.Stop();
        Logger::LogInfo("Application ran for " + std::to_string(stopwatch.GetElapsedSec()) + " seconds");
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