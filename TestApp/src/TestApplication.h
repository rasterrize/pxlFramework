#include <pxl/pxl.h>

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication();
        virtual ~TestApplication() override = default;

        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGUIRender() override;
        virtual void OnClose() override;

    private:
        std::function<void(pxl::WindowSpecs&)> m_OnStartFunc = nullptr;
        std::function<void(float dt)> m_OnUpdateFunc = nullptr;
        std::function<void()> m_OnRenderFunc = nullptr;
        std::function<void()> m_OnGUIRenderFunc = nullptr;
        std::function<void()> m_OnCloseFunc = nullptr;
        std::function<std::shared_ptr<pxl::Window>()> m_GetWindowFunc = nullptr;
    };
}