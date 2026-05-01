#include "Application.h"

#include "Events/EventHandler.h"
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Window.h"

#ifdef PXL_DEBUG
    #define PXL_FRAMEWORK_CONFIG_FILE_NAME "Framework-dev.ini"
#else
    #define PXL_FRAMEWORK_CONFIG_FILE_NAME "Framework.ini"
#endif

namespace pxl
{
    Application::Application()
    {
        PXL_PROFILE_SCOPE;

        if (s_Instance)
            throw std::runtime_error("Failed to create application object because one already exists. This likely indicates a bug within the user's application.");

        s_Instance = this;

        PXL_INIT_LOGGING;

        InitPlatformingBackend();

        m_EventManager = std::make_unique<EventManager>();

        m_FrameworkIni = std::make_unique<IniConfig>(PXL_FRAMEWORK_CONFIG_FILE_NAME, DefaultFrameworkSettings());

#define PXL_ENABLE_DEBUG_OVERLAY
#ifdef PXL_ENABLE_DEBUG_OVERLAY
        m_DebugOverlay = std::make_unique<DebugOverlay>();
        PXL_CREATE_AND_REGISTER_HANDLER(m_KeyDownHandler, KeyDownEvent, OnKeyDownEvent);
#endif
    }

    Application::~Application()
    {
        // Explicitly shutdown all systems
        m_GamepadManager.reset();
        ShutdownRenderer();
        Window::Shutdown();
        m_FrameworkIni.reset();
        ShutdownPlatformingBackend();

        s_Instance = nullptr;

        PXL_LOG_INFO("Application destroyed ~ goodbye!");
    }

    void Application::Run()
    {
        while (m_Running)
        {
            PXL_PROFILE_SCOPE;

            Stopwatch updateSW;
            m_UpdateStartPoint = std::chrono::steady_clock::now();

            // Calculate deltaTime
            auto time = static_cast<float>(Platform::GetTimeRunning());
            auto deltaTime = time - m_PlatformTimeLastCycle;
            m_PlatformTimeLastCycle = time;

            if (Window::IsInitialized())
                Window::ProcessEvents();

            if (m_GamepadManager)
                m_GamepadManager->ProcessStateChanges();

            m_EventManager->ProcessQueue();

            // Stop processing if the app was closed by events (WindowClose, KeyDown, etc)
            if (!m_Running)
                break;

            OnUpdate(deltaTime);

            // NOTE: C++ uses short circuit evaluation, so this is valid
            if (m_Renderer && !m_Renderer->IsSuspended())
            {
                m_Renderer->Begin();
                OnRender(*m_Renderer);
#ifdef PXL_ENABLE_IMGUI
                if (m_Renderer->IsImGuiInitialized())
                {
                    OnGUIRender();
                    if (m_DebugOverlay)
                        m_DebugOverlay->Render(*m_MainWindow, *m_Renderer);
                }
#endif
                m_Renderer->End();

                m_Renderer->LimitFramerateIfNecessary();
                m_Renderer->m_FrameStats.FrameTime = updateSW.GetElapsedMilliSec();
            }

            PXL_PROFILE_FRAME_END;
        }

        OnClose();
    }

    void Application::Close()
    {
        m_Running = false;
        PXL_LOG_INFO("Application closing...");
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

    GamepadManager& Application::InitGamepadManager()
    {
        m_GamepadManager = std::make_unique<GamepadManager>(m_EventManager->GetEventQueueCallback());
        return *m_GamepadManager;
    }

    const std::shared_ptr<Window>& Application::InitMainWindow(WindowSpecs& specs, bool overrideWithIni)
    {
        if (overrideWithIni)
            OverrideWithFrameworkIni(specs);

        m_MainWindow = Window::Create(specs);
        PXL_CREATE_AND_REGISTER_HANDLER(m_WindowCloseHandler, WindowCloseEvent, OnWindowCloseEvent);
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
        PXL_LOG_INFO("GLFW initialized (v{}.{}.{})", major, minor, rev);

        std::string platform = "Unknown";
#ifdef _WIN32
        platform = "Windows";
#endif
#ifdef __linux__
        std::string graphicsPlatform = "Unknown";

        if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
            graphicsPlatform = "Wayland";
        else if (glfwGetPlatform() == GLFW_PLATFORM_X11)
            graphicsPlatform = "X11";

        platform = std::format("Linux ({})", graphicsPlatform);
#endif

        PXL_LOG_INFO("Platform: {}", platform);
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

    void Application::OnKeyDownEvent(KeyDownEvent& e)
    {
        if (e.IsModsAndKey(KeyMod::Control, KeyCode::F11))
            m_DebugOverlay->ToggleVisibility();
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