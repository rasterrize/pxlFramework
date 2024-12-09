#pragma once

namespace pxl
{
    class Application
    {
    public:
        Application();

        // Delete copy constructors
        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;

        virtual ~Application();

        void Run();
        void Close();

        bool IsRunning() const { return m_Running; }

        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;
        virtual void OnGUIRender() = 0;
        virtual void OnClose() = 0;

        void SetMinimization(bool minimized) { m_Minimized = minimized; }
        void SetFPSLimit(uint32_t limit) { m_FPSLimit = limit; }

        static Application& Get()
        {
            PXL_ASSERT(s_Instance);
            return *s_Instance;
        }

    private:
        void LimitFPS();

    private:
        bool m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;
        std::chrono::steady_clock::time_point m_FrameStartTime;
        uint32_t m_FPSLimit = 0;

        static inline Application* s_Instance = nullptr;
    };
}