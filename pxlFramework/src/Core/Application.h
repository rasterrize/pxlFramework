namespace pxl
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Run();
        void Close();

        bool IsRunning() 
        { 
            if (s_Instance)
            {
                return m_Running; 
            }
        }

        static Application& Get() { return *s_Instance; }

        virtual void OnUpdate() = 0;
    private:
        bool m_Running = true;

        static Application* s_Instance;
    };
}