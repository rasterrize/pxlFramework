#include <pxl/pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        virtual ~TestApplication() override;

        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGUIRender() override;

    private:
        std::function<void(pxl::WindowSpecs&)> m_OnStartFunc = nullptr;
        std::function<void(float dt)> m_OnUpdateFunc = nullptr;
        std::function<void()> m_OnRenderFunc = nullptr;
        std::function<void()> m_OnGUIRenderFunc = nullptr;
    };
}