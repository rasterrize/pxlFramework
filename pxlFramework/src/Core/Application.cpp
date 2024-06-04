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
            throw; // should this be abort() or exit()?
        }

        s_Instance = this;

        // Not sure if these 'Framework' classes should be in the 'Application' class
        FrameworkConfig::Init();
    }

    void Application::Run()
    {
        Stopwatch stopwatch;
        
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
                Renderer::Begin(); // should Begin and End be called if the renderer isn't enabled?
                OnRender();
                GUI::Update();
                Renderer::End();
            }

            Window::UpdateAll();

            PXL_PROFILE_FRAME_END;
        }

        stopwatch.Stop();
        PXL_LOG_INFO(LogArea::Core, "Application ran for {} seconds", stopwatch.GetElapsedSec());
    }

    void Application::Close()
    {
        if (m_Running)
        {
            m_Running = false; // this must be set first so window or input don't call the close function as well
            Window::Shutdown(); // could these be put into the test app destructor to reduce overhead?
            Renderer::Shutdown();
            Input::Shutdown();  
            GUI::Shutdown();
            s_Instance = nullptr;
        }
    }
}