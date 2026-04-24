#include "Application.h"

#include "Config.h"
#include "Input.h"
#include "Events/EventHandler.h"
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Window.h"

using namespace std::literals;

namespace pxl
{
    Application::Application()
    {
        PXL_PROFILE_SCOPE;

        if (s_Instance)
            throw std::runtime_error("Failed to create application object because one already exists. This likely indicates a bug within the user's application.");

        s_Instance = this;

        PXL_INIT_LOGGING;

        FrameworkConfig::Init();
        InitPlatformingBackend();

        m_EventManager = std::make_unique<EventManager>();
    }

    Application::~Application()
    {
        FrameworkConfig::Shutdown();
        ShutdownRenderer();
        Input::Shutdown();
        Window::Shutdown();
        ShutdownPlatformingBackend();

        s_Instance = nullptr;

        PXL_LOG_INFO(LogArea::Core, "Application destroyed ~ goodbye!");
    }

    void Application::Run()
    {
        while (m_Running)
        {
            PXL_PROFILE_SCOPE;

            auto startPoint = std::chrono::steady_clock::now();
            m_Renderer->m_FrameStats.FrameTime = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_UpdateStartPoint).count();
            m_UpdateStartPoint = startPoint;

            // Calculate deltaTime
            auto time = static_cast<float>(Platform::GetTimeRunning());
            auto deltaTime = time - m_PlatformTimeLastCycle;
            m_PlatformTimeLastCycle = time;

            if (Window::IsInitialized())
                Window::ProcessEvents();

            m_EventManager->ProcessQueue();

            if (m_Running)
            {
                OnUpdate(deltaTime);

                // NOTE: C++ uses short circuit evaluation, so this is valid
                if (m_Renderer && !m_Renderer->IsSuspended())
                {
                    m_Renderer->Begin();
                    OnRender(*m_Renderer);
#ifdef PXL_ENABLE_IMGUI
                    if (m_Renderer->IsImGuiInitialized())
                        OnGUIRender();
#endif

                    m_Renderer->End();
                    m_Renderer->LimitFramerateIfNecessary();
                }
            }

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

    Renderer& Application::InitRenderer(RendererConfig& config, bool overrideWithIni)
    {
        if (overrideWithIni)
            OverrideWithFrameworkIni(config);

        m_Renderer = std::make_unique<Renderer>(config);
        return *m_Renderer;
    }

    void Application::ShutdownRenderer()
    {
        m_Renderer.reset();
    }

    const std::shared_ptr<Window>& Application::InitMainWindow(WindowSpecs& specs, bool overrideWithIni)
    {
        if (overrideWithIni)
            OverrideWithFrameworkIni(specs);

        m_MainWindow = Window::Create(specs);
        PXL_CREATE_AND_REGISTER_HANDLER(m_WindowCloseEventHandler, WindowCloseEvent, OnWindowCloseEvent);
        return m_MainWindow;
    }

    void Application::OverrideWithFrameworkIni(WindowSpecs& specs)
    {
        specs.WindowMode = Utils::ToWindowMode(m_FrameworkIni->GetValue("Window", "WindowMode"));

        // TODO
    }

    void Application::OverrideWithFrameworkIni(RendererConfig&)
    {
        // TODO
    }

    void Application::InitPlatformingBackend()
    {
        PXL_PROFILE_SCOPE;

        bool result = glfwInit();

        if (!result)
            throw std::runtime_error("GLFW failed to initialize");

        int major = 0, minor = 0, rev = 0;
        glfwGetVersion(&major, &minor, &rev);
        PXL_LOG_INFO(LogArea::Window, "GLFW initialized - Version {}.{}.{}", major, minor, rev);
    }

    void Application::ShutdownPlatformingBackend()
    {
        glfwTerminate();
    }

    void Application::OnWindowCloseEvent(const WindowCloseEvent& e)
    {
        if (e.GetWindow() == m_Renderer->GetConfig().Window)
            ShutdownRenderer();

        if (e.GetWindow() == m_MainWindow)
            Close();
    }

    std::vector<IniConfigSetting> Application::DefaultFrameworkSettings()
    {
        return {
            { "Renderer", "GraphicsAPI", Utils::ToString(RendererConstants::DefaultGraphicsAPI) },
            { "Renderer", "VerticalSync", std::format("{}", RendererConstants::DefaultVerticalSync) },
            { "Renderer", "AllowTearing", std::format("{}", RendererConstants::DefaultAllowTearing) },
            { "Renderer", "TripleBuffering", std::format("{}", RendererConstants::DefaultTripleBuffering) },
            { "Renderer", "FramerateMode", Utils::ToString(RendererConstants::DefaultFramerateMode) },
            { "Window", "WindowMode", Utils::ToString(WindowConstants::DefaultWindowMode) },
        };
    }
}