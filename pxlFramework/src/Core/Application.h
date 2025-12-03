#pragma once

#include "Events/EventManager.h"

namespace pxl
{
    enum class FramerateMode
    {
        Unlimited,
        Custom,
        GSYNC,
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

        // Override these methods in a derived class to add custom logic
        virtual void OnUpdate(float dt) {}
        virtual void OnRender() {}
        virtual void OnGUIRender() {} // This function only gets called if ImGui is initialized
        virtual void OnClose() {}
        virtual void OnEvent(const Event& e) {}

        void SetMinimization(bool minimized) { m_Minimized = minimized; }

        uint32_t GetFPSLimit() const { return m_CustomFPSLimit; }
        void SetFPSLimit(uint32_t limit) { m_CustomFPSLimit = limit; }

        FramerateMode GetFramerateMode() const { return m_FramerateMode; }
        void SetFramerateMode(FramerateMode mode);

        const std::unique_ptr<EventManager>& GetEventManager() const { return m_EventManager; }

        static Application& Get()
        {
            PXL_ASSERT(s_Instance);
            return *s_Instance;
        }

        static bool Exists() { return s_Instance; }

    private:
        void LimitFPS();

    private:
        bool m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;
        std::chrono::steady_clock::time_point m_FrameStartTime;
        uint32_t m_CustomFPSLimit = 60; // TODO: make constant with config.h
        uint32_t m_GsyncFPSLimit = 0;
        FramerateMode m_FramerateMode = FramerateMode::Unlimited;

        std::unique_ptr<EventManager> m_EventManager = nullptr;

        static inline Application* s_Instance = nullptr;
    };
}