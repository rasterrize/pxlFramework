namespace pxl
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void Close();
    private:
        bool m_Running = true;
    };
}