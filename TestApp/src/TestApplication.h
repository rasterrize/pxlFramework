#pragma once

#include <pxl/pxl.h>

#include "Tests/Test.h"

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        TestApplication(const std::vector<std::string>& args);

        virtual void OnUpdate(float dt) override;
        virtual void OnRender(pxl::Renderer& renderer) override;
        virtual void OnClose() override;
        virtual void OnEvent(pxl::Event& e) override;

#ifdef PXL_ENABLE_IMGUI
        virtual void OnGUIRender() override;
#endif

        template<typename TestT>
        void LaunchTest()
        {
            if (!std::is_base_of_v<Test, TestT>)
            {
                APP_LOG_CRITICAL("LaunchTest called with invalid test type");
                return;
            }

            m_Test = std::make_unique<TestT>();
            auto specs = CreateWindowSpecs();
            m_Test->OnStart(specs);
        }

        bool HasTest() const { return static_cast<bool>(m_Test); }

    private:
        pxl::WindowSpecs CreateWindowSpecs();
        void OnKeyDownEvent(const pxl::KeyDownEvent& e);

    private:
        std::unique_ptr<Test> m_Test;

        bool m_ShowMainMenu = true;
        bool m_ShowImGui = true;

        pxl::UserEventHandler<pxl::KeyDownEvent> m_KeyDownHandler;

        pxl::RendererConfig m_DefaultRendererConfig = {};
    };
}