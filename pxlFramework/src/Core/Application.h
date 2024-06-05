#pragma once

namespace pxl
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Run();
        void Close();

        const bool IsRunning() const { return m_Running; }

        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;
        virtual void OnImGuiRender() = 0;

        static Application& Get() { return *s_Instance; }

        // should be accessible by only window class
        void SetMinimization(bool minimized) { m_Minimized = minimized; }
    private:
        bool m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;

        static Application* s_Instance;
    };
}