#include <pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        virtual ~TestApplication() override;

        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGuiRender() override;
    private:
        std::function<void(pxl::WindowSpecs&)> m_OnStartFunc;
        std::function<void(float dt)> m_OnUpdateFunc;
        std::function<void()> m_OnRenderFunc;
        std::function<void()> m_OnGuiRender;
    };
}