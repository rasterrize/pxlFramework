#include "Application.h"

#include "Window.h"
#include "Input.h"
#include "Platform.h"
#include "Stopwatch.h"
#include "Config.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Camera.h"
#include "../Debug/ImGui/pxl_ImGui.h"

namespace pxl
{
    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        if (s_Instance)
        {
            PXL_LOG_ERROR(LogArea::Core, "Can't create application, one already exists");
            throw std::runtime_error("Failed to create application object because one already exists. This likely indicates a bug within the application.");
        }

        s_Instance = this;

        FrameworkConfig::Init();
    }

    void Application::Run()
    {
        while (m_Running)
        {
            PXL_PROFILE_SCOPE;

            float time = static_cast<float>(Platform::GetTime());
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                OnUpdate(deltaTime);
                
                if (!m_Running)
                    break;

                Camera::UpdateAll();
                Renderer::Begin();
                OnRender();
                GUI::Update();
                Renderer::End();
            }

            Window::UpdateAll();

            PXL_PROFILE_FRAME_END;
        }
    }

    void Application::Close()
    {
        m_Running = false;
        
        Input::Shutdown();
        Window::Shutdown();
        Renderer::Shutdown();
        GUI::Shutdown();

        s_Instance = nullptr;

        PXL_LOG_INFO(LogArea::Core, "Application closed");
    }
}