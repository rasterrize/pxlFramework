#pragma once

#include "Events/EventManager.h"
#include "Renderer/Renderer.h"

namespace pxl
{
    enum class FramerateMode
    {
        Unlimited,
        Custom,
        AdaptiveSync,
    };

    /** @brief The core that runs a pxlFramework application.
        IMPORTANT: You must inherit from this class and override specific methods (OnUpdate, OnRender, etc) to add anything custom to your app.
    */
    class Application
    {
    public:
        Application();

        // Delete copy constructors
        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;

        // Enforces this class as abstract
        virtual ~Application() = 0;

        void Run();
        void Close();

        bool IsRunning() const { return m_Running; }

        /* Called every update cycle for this application.
           Use this function for application/game logic. */
        virtual void OnUpdate(float dt) {}

        /* Called every frame for this application's renderer. Will only be called if the renderer is initialized.
           Use this function for any kind of rendering. */
        virtual void OnRender(const std::unique_ptr<Renderer>& renderer) {}

        // Called every frame. Convenience function for holding ImGui related functions.
        virtual void OnGUIRender() {}

        // Called once when the application closes.
        virtual void OnClose() {}

        // Called for every event that passes through the event system. Only use this in special cases.
        virtual void OnEvent(const Event& e) {}

        const std::unique_ptr<Renderer>& GetRenderer() const { return m_Renderer; }
        const std::unique_ptr<Renderer>& InitRenderer(const RendererConfig& config);
        void ShutdownRenderer();

        void SetMinimization(bool minimized) { m_Minimized = minimized; }

        uint32_t GetFPSLimit() const { return m_CustomFPSLimit; }
        void SetFPSLimit(uint32_t limit) { m_CustomFPSLimit = limit; }

        FramerateMode GetFramerateMode() const { return m_FramerateMode; }
        void SetFramerateMode(FramerateMode mode);

        const std::unique_ptr<EventManager>& GetEventManager() const { return m_EventManager; }

    public:
        static Application& Get()
        {
            PXL_ASSERT(s_Instance);
            return *s_Instance;
        }

        static bool Exists() { return s_Instance; }

    private:
        void LimitFPS();

    private:
        static inline Application* s_Instance = nullptr;

        bool m_Running = true;
        bool m_Minimized = false;

        float m_LastFrameTime = 0.0f;
        std::chrono::steady_clock::time_point m_FrameStartTime;
        uint32_t m_CustomFPSLimit = 60; // TODO: make constant with config.h
        uint32_t m_AdaptiveSyncFPSLimit = 0;
        FramerateMode m_FramerateMode = FramerateMode::Unlimited;

        std::unique_ptr<Renderer> m_Renderer = nullptr;
        std::unique_ptr<EventManager> m_EventManager = nullptr;
    };
}