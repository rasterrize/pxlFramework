#pragma once

#include "Events/EventManager.h"

namespace pxl
{
    template<typename EventT>
    class EventHandler;

    class Renderer;
    struct RendererConfig;

    namespace ApplicationConstants
    {
        static const uint32_t k_DefaultCustomFPSLimit = 60;
    }

    enum class FramerateMode
    {
        Unlimited,
        Custom,
        AdaptiveSync,
    };

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
        void Close();

        bool IsRunning() const { return m_Running; }

        /// @brief Called once every update cycle for this application.
        /// Use this function for application/game logic.
        /// @param dt The deltaTime between the start of the last update cycle and the start of the current update cycle.
        /// This can be used to sync variables correctly when the frame rate is fluctuating (e.g uncapped).
        virtual void OnUpdate(float dt) {}

        /// @brief Called once every frame. Use this function for any kind of rendering purposes.
        /// @note Will only be called if this application's renderer is initialized.
        /// @param renderer The renderer to submit renderer operations to.
        virtual void OnRender(pxl::Renderer& renderer) {}

        /// @brief Called once every frame. Convenience function for holding ImGui related functions.
        /// @note Will only be called if this application's renderer has its ImGui renderer initialized.
        virtual void OnGUIRender() {}

        /// @brief Called once when the application closes.
        virtual void OnClose() {}

        /// @brief Called once per every event that passes through the event system. Only use this in special cases.
        virtual void OnEvent(const Event& e) {}

        Renderer& GetRenderer() const { return *m_Renderer; }

        /// @brief Initializes the renderer for the application.
        /// @param config The configuration of the renderer.
        /// @return A reference to the renderer object to perform any additional configurations with the renderer.
        Renderer& InitRenderer(const RendererConfig& config);
        void ShutdownRenderer();

        void SetMinimization(bool minimized) { m_Minimized = minimized; }

        uint32_t GetFPSLimit() const { return m_CustomFPSLimit; }
        void SetFPSLimit(uint32_t limit) { m_CustomFPSLimit = limit; }

        FramerateMode GetFramerateMode() const { return m_FramerateMode; }
        void SetFramerateMode(FramerateMode mode);

        const std::unique_ptr<EventManager>& GetEventManager() const { return m_EventManager; }

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
        void LimitFPS();
        void InitPlatformingBackend();
        void ShutdownPlatformingBackend();

    private:
        static inline Application* s_Instance = nullptr;

        bool m_Running = true;
        bool m_Minimized = false;

        std::unique_ptr<Renderer> m_Renderer;
        std::unique_ptr<EventManager> m_EventManager;

        float m_LastFrameTime = 0.0f;
        std::chrono::steady_clock::time_point m_FrameStartTime;
        uint32_t m_CustomFPSLimit = ApplicationConstants::k_DefaultCustomFPSLimit;
        uint32_t m_AdaptiveSyncFPSLimit = 0;
        FramerateMode m_FramerateMode = FramerateMode::Unlimited;
    };

    namespace Utils
    {
        inline std::string ToString(FramerateMode mode)
        {
            switch (mode)
            {
                case FramerateMode::Unlimited:    return "Unlimited";
                case FramerateMode::Custom:       return "Custom";
                case FramerateMode::AdaptiveSync: return "Adaptive Sync";
                default:                          return "Unknown";
            }
        }
    }
}