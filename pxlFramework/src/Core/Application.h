#pragma once

#include "Core/IniConfig.h"
#include "Events/EventManager.h"
#include "Events/WindowEvents.h"
#include "Window.h"

namespace pxl
{
    template<typename EventT>
    class EventHandler;

    class Renderer;
    struct RendererConfig;

    /// @brief The core that runs a pxlFramework application.
    /// @note You must inherit from this class and override specific methods (OnUpdate, OnRender, etc) to add anything custom to your app.
    class Application
    {
    public:
        Application();

        // Delete copy constructors
        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;

        // Enforces this class as abstract
        virtual ~Application() = 0;

        /// @brief Begins the running loop of the application. Only returns once the application closes.
        /// It's recommended to call this from your main function.
        void Run();

        /// @brief Immediately closes the application, force shutting down all internal systems.
        /// Remember to return from your function after calling this to avoid interfacing into invalid systems.
        void Close();

        bool IsRunning() const { return m_Running; }

        /// @brief Called once every update cycle for this application.
        /// Use this function for application/game logic.
        /// @param dt The deltaTime between the start of the last update cycle and the start of the current update cycle.
        /// This can be used to sync variables independently of frame rate.
        virtual void OnUpdate([[maybe_unused]] float dt) {}

        /// @brief Called once every frame. Use this function for any kind of rendering purposes.
        /// @note Will only be called if this application's renderer is initialized.
        /// @param renderer The renderer to submit renderer operations to.
        virtual void OnRender([[maybe_unused]] pxl::Renderer& renderer) {}

        /// @brief Called once every frame. Convenience function for holding ImGui related functions.
        /// @note Will only be called if this application's renderer has its ImGui renderer initialized.
        virtual void OnGUIRender() {}

        /// @brief Called once when the application closes.
        virtual void OnClose() {}

        /// @brief Called once per every event that passes through the event system. Only use this in special cases.
        virtual void OnEvent([[maybe_unused]] Event& e) {}

        const std::shared_ptr<Window>& InitMainWindow(WindowSpecs& specs, bool overrideWithIni = true);
        const std::shared_ptr<Window>& GetMainWindow() const { return m_MainWindow; }
        void SetMainWindow(const std::shared_ptr<Window>& window) { m_MainWindow = window; }

        /// @brief Initializes the renderer for the application.
        /// @param config The configuration of the renderer.
        /// @return A reference to the renderer object to perform any additional configurations with the renderer.
        Renderer& InitRenderer(RendererConfig& config, bool overrideWithIni = true);
        Renderer& GetRenderer() const { return *m_Renderer; }
        void ShutdownRenderer();

        const std::chrono::steady_clock::time_point& GetUpdateStartPoint() const { return m_UpdateStartPoint; }

        EventManager& GetEventManager() const { return *m_EventManager; }

        void OverrideWithFrameworkIni(WindowSpecs& specs);
        void OverrideWithFrameworkIni(RendererConfig& config);

    public:
        /// @brief Gets the singleton instance of the application class.
        /// @return A reference to the application instance.
        static Application& Get()
        {
            PXL_ASSERT(s_Instance);
            return *s_Instance;
        }

        static bool Exists() { return s_Instance; }

    private:
        void InitPlatformingBackend();
        void ShutdownPlatformingBackend();

        void OnWindowCloseEvent(const WindowCloseEvent& e);

        std::vector<IniConfigSetting> DefaultFrameworkSettings();

    private:
        static inline Application* s_Instance = nullptr;

        bool m_Running = true;

        std::unique_ptr<Renderer> m_Renderer;
        std::shared_ptr<Window> m_MainWindow;
        std::unique_ptr<EventManager> m_EventManager;
        std::unique_ptr<IniConfig> m_FrameworkIni;

        std::shared_ptr<EventHandler<WindowCloseEvent>> m_WindowCloseEventHandler;

        float m_PlatformTimeLastCycle = 0.0f;
        std::chrono::steady_clock::time_point m_UpdateStartPoint = {};
    };
}