#include "Application.h"

#include "Config.h"
#include "Debug/GUI/GUI.h"
#include "Input.h"
#include "Platform.h"
#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
#include "Stopwatch.h"
#include "Window.h"

using namespace std::literals;

namespace pxl
{
    Application::Application()
    {
        PXL_PROFILE_SCOPE;

        if (s_Instance)
            throw std::runtime_error("Failed to create application object because one already exists. This likely indicates a bug within the application.");

        s_Instance = this;

        PXL_INIT_LOGGING;

        FrameworkConfig::Init();

        m_EventManager = std::make_unique<EventManager>();
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
            m_FrameStartTime = std::chrono::steady_clock::now();

            float time = static_cast<float>(Platform::GetTime());
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            Window::ProcessEvents();
            m_EventManager->ProcessQueue();

            if (!m_Minimized && m_Running)
            {
                OnUpdate(deltaTime);

                if (Renderer::IsInitialized())
                {
                    Camera::UpdateAll();
                    Renderer::Begin();
                    OnRender();
                    Renderer::End();
                }
            }

            Window::UpdateAll();

            // Limit the frame rate if necessary
            if (m_FramerateMode != FramerateMode::Unlimited)
                LimitFPS();

            Renderer::s_FrameCount++;
            Renderer::CalculateFPS();

            PXL_PROFILE_FRAME_END;
        }
    }

    void Application::Close()
    {
        if (!m_Running)
            return;

        m_Running = false;

        PXL_LOG_INFO(LogArea::Core, "Application closing...");

        OnClose();

        FrameworkConfig::Shutdown();
        GUI::Shutdown();
        Renderer::Shutdown();
        Input::Shutdown();
        Window::Shutdown();
    }

    void Application::SetFramerateMode(FramerateMode mode)
    {
        if (mode == FramerateMode::GSYNC)
        {
            if (Window::IsInitialized())
            {
                m_GsyncFPSLimit = Window::GetPrimaryMonitor().GetCurrentVideoMode().RefreshRate - 3;
            }
            else
            {
                PXL_LOG_WARN(LogArea::Window, "Couldn't retrieve GSYNC fps limit, defaulting to 0");
                m_GsyncFPSLimit = 0;
            }
        }

        m_FramerateMode = mode;
    }

    void Application::LimitFPS()
    {
        auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_FrameStartTime);
        uint32_t limit = 0;

        // TODO: use a function to return a suitable limit, and skip the next steps if its 0 (unlimited)
        if (m_FramerateMode == FramerateMode::Custom)
            limit = m_CustomFPSLimit;
        else if (m_FramerateMode == FramerateMode::GSYNC)
            limit = m_GsyncFPSLimit;

        auto us = std::chrono::microseconds(1s) / limit;
        auto overhead = us - frameTime;

        if (overhead > 0us)
        {
            // TODO: Stopwatch uses high resolution clock, which can go backwards, and therefore breaks if the time is changed on the OS
            // To fix, this should be using a steady clock stopwatch variant
            Stopwatch wait;
            while (wait.GetElapsedMicroSec() < overhead.count())
            {
            }
        }
    }
}