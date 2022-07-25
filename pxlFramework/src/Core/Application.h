namespace pxl
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void Close();

        virtual void OnUpdate() = 0;
    private:
        bool m_Running = true;
    };
}