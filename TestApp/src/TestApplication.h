#pragma once

#include <pxl/pxl.h>

#include "Tests/Test.h"

namespace TestApp
{
    class TestApplication : public pxl::Application
    {
    public:
        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGUIRender() override;
        virtual void OnClose() override;

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

    private:
        pxl::WindowSpecs CreateWindowSpecs();

    private:
        std::unique_ptr<Test> m_Test = nullptr;
    };
}