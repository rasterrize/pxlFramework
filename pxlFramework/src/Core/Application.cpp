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

        // Manually set the process's timer resolution to 1ms so the FPS limiter works more accurately.
        Platform::SetMinimumTimerResolution(1);
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

            // Limit the frame rate by sleeping the thread
            if (m_FPSLimit > 0)
                LimitFPS();

            Renderer::s_FrameCount++;
            Renderer::CalculateFPS();

            PXL_PROFILE_FRAME_END;
        }
    }

    void Application::Close()
    {
        m_Running = false;

        PXL_LOG_INFO(LogArea::Core, "Application closing...");

        OnClose();

        if (FrameworkConfig::s_AutoSave)
            FrameworkConfig::SaveToFile();
        Platform::ResetMinimumTimerResolution(1);

        GUI::Shutdown();
        Renderer::Shutdown();
        Input::Shutdown();
        Window::Shutdown();
    }

    void Application::LimitFPS()
    {
        auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_FrameStartTime);
        auto us = std::chrono::microseconds(1s) / m_FPSLimit;
        auto overhead = us - frameTime;

        // TODO: Sleep() can be very inaccurate, so I need to research the modern approach for this. THANKS MICROSOFT.
        if (overhead > 0ms)
            Sleep(static_cast<DWORD>(overhead.count() / 1000));
    }
}