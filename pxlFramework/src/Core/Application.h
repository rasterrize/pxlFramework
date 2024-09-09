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

        void SetMinimization(bool minimized) { m_Minimized = minimized; }

        static Application& Get()
        {
            PXL_ASSERT(s_Instance);
            return *s_Instance;
        }
    private:
        bool m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;

        static inline Application* s_Instance = nullptr;
    };
}