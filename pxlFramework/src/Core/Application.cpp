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
            throw;
        }
        s_Instance = this;

        // Not sure if these 'Framework' classes should be in the 'Application' class
        FrameworkConfig::Init();

        #ifndef PXL_DISABLE_LOGGING
            Logger::Init();
        #endif
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
                if (!m_Running)
                    break;
                Camera::UpdateAll();
                Renderer::Begin(); // should Begin and End be called if the renderer isn't enabled?
                OnRender();
                pxl_ImGui::Update();
                Renderer::End();
            }

            Window::UpdateAll();
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
            pxl_ImGui::Shutdown();
            s_Instance = nullptr;
        }
    }
}