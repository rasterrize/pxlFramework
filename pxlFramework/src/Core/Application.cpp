#include "Application.h"

#include "Config.h"
#include "Debug/GUI/GUI.h"
#include "Input.h"
#include "Platform.h"
#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
#include "Stopwatch.h"
#include "Utils/DiscordRPC.h"
#include "Window.h"

namespace pxl
{
    Application::Application()
    {
        PXL_PROFILE_SCOPE;

        if (s_Instance)
            throw std::runtime_error("Failed to create application object because one already exists. This likely indicates a bug within the application.");

        s_Instance = this;

        FrameworkConfig::Init();
        Window::Init();
    }

    Application::~Application()
    {
        s_Instance = nullptr;

        PXL_LOG_INFO(LogArea::Core, "Application destroyed");
    }

    void Application::Run()
    {
        while (m_Running)
        {
            PXL_PROFILE_SCOPE;

            float time = static_cast<float>(Platform::GetTime());
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            Window::ProcessEvents();

            // If all windows are closed after window processes close events, stop running
            if (!m_Running)
                break;

            if (!m_Minimized)
            {
                if (Input::IsInitialized())
                    Input::Update();

                OnUpdate(deltaTime);

                // If user application closes the app manually, stop running
                if (!m_Running)
                    break;

                Camera::UpdateAll();
                Renderer::Begin();
                OnRender();
                Renderer::End();
                DiscordRPC::Update();
            }

            Window::UpdateAll();

            Renderer::s_FrameCount++;
            Renderer::CalculateFPS();

            PXL_PROFILE_FRAME_END;
        }
    }

    void Application::Close()
    {
        m_Running = false;

        PXL_LOG_INFO(LogArea::Core, "Application closing...");

        GUI::Shutdown();
        Renderer::Shutdown();
        Input::Shutdown();
        Window::Shutdown();
    }
}