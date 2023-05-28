#include <pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        ~TestApplication();

        virtual void OnUpdate() override;
    private:
        std::shared_ptr<pxl::Window> m_Window;
    };
}