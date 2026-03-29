#include "Application.h"

#include "Config.h"
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
        FrameworkConfig::Shutdown();
        ShutdownRenderer();
        Input::Shutdown();
        Window::Shutdown();

        s_Instance = nullptr;

        PXL_LOG_INFO(LogArea::Core, "Application destroyed");
    }

    void Application::Run()
    {
        while (m_Running)
        {
            PXL_PROFILE_SCOPE;

            m_FrameStartTime = std::chrono::steady_clock::now();

            // Calculate deltaTime
            float time = static_cast<float>(Platform::GetTime());
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (Window::IsInitialized())
                Window::ProcessEvents();

            m_EventManager->ProcessQueue();

            if (m_Running)
            {
                OnUpdate(deltaTime);

                if (m_Renderer)
                {
                    Camera::UpdateAll(); // TODO: Do something else here
                    m_Renderer->Begin();
                    OnRender(*m_Renderer.get());

#ifdef PXL_ENABLE_IMGUI
                    if (m_Renderer->IsImGuiInitialized())
                        OnGUIRender();
#endif

                    m_Renderer->End();
                }
            }

            Window::UpdateAll();

            // Limit the frame rate if necessary
            if (m_FramerateMode != FramerateMode::Unlimited)
                LimitFPS();

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
    }

    Renderer& Application::InitRenderer(const RendererConfig& config)
    {
        m_Renderer = std::make_unique<Renderer>(config);
        return *m_Renderer;
    }

    void Application::ShutdownRenderer()
    {
        m_Renderer.reset();
    }

    void Application::SetFramerateMode(FramerateMode mode)
    {
        if (mode == FramerateMode::AdaptiveSync)
        {
            if (Window::IsInitialized())
            {
                m_AdaptiveSyncFPSLimit = Window::GetPrimaryMonitor().GetCurrentVideoMode().RefreshRate - 3;
            }
            else
            {
                PXL_LOG_WARN(LogArea::Window, "Couldn't retrieve AdaptiveSync fps limit, defaulting to 0");
                m_AdaptiveSyncFPSLimit = 0;
            }
        }

        m_FramerateMode = mode;
    }

    void Application::LimitFPS()
    {
        auto elapsedAlready = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - m_FrameStartTime);

        uint32_t limit = 0;
        if (m_FramerateMode == FramerateMode::Custom)
            limit = m_CustomFPSLimit;
        else if (m_FramerateMode == FramerateMode::AdaptiveSync)
            limit = m_AdaptiveSyncFPSLimit;

        auto nsToWait = std::chrono::nanoseconds(1s) / limit;

        while ((nsToWait - elapsedAlready) > 0ns)
        {
            elapsedAlready = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - m_FrameStartTime);
        }
    }
}