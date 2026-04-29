#include "TestApplication.h"

#include "Tests/CubesTest.h"
#include "Tests/EmptyApp.h"
#include "Tests/LinesTest.h"
#include "Tests/ModelViewer.h"
#include "Tests/MultiWindow.h"
#include "Tests/QuadsTest.h"

#define TA_DEFAULT_TEST QuadsTest

namespace TestApp
{
    TestApplication::TestApplication(const std::vector<std::string>& args)
    {
        // Parse launch arguments
        for (auto& string : args)
        {
            // Check if this is a valid argument
            if (string.front() != '-')
                continue;

            if (string.at(1) == 't')
            {
                auto testValue = string.substr(sizeof("-t"), string.length());

                // TODO: simplify this
                // if (testValue == "ModelViewer")
                //     LaunchTest<TestApp::ModelViewer>();
                if (testValue == "QuadsTest")
                    LaunchTest<TestApp::QuadsTest>();
                // else if (testValue == "CubesTest")
                //     LaunchTest<TestApp::CubesTest>();
                // else if (testValue == "LinesTest")
                //     LaunchTest<TestApp::LinesTest>();
                // else if (testValue == "EmptyApp")
                //     LaunchTest<TestApp::EmptyApp>();
                // else if (testValue == "MultiWindow")
                //     LaunchTest<TestApp::MultiWindow>();
            }
        }

        if (!m_Test)
            LaunchTest<TestApp::TA_DEFAULT_TEST>();

        PXL_CREATE_AND_REGISTER_HANDLER(m_KeyDownHandler, pxl::KeyDownEvent, OnKeyDownEvent);
    }

    void TestApplication::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnUpdate(dt);
    }

    void TestApplication::OnRender(pxl::Renderer& renderer)
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnRender(renderer);
    }

    void TestApplication::OnClose()
    {
        if (m_Test)
            m_Test->OnClose();
    }

    void TestApplication::OnEvent(pxl::Event& e)
    {
#define TA_LOG_ALL_EVENTS 0
#if TA_LOG_ALL_EVENTS
        APP_LOG_INFO("{}", e.ToString());
#endif
    }

#ifdef PXL_ENABLE_IMGUI
    void TestApplication::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        if (!m_ShowImGui)
            return;

        if (m_Test)
            m_Test->OnGUIRender();
    }
#endif

    pxl::WindowSpecs TestApplication::CreateWindowSpecs()
    {
        std::string buildTypeString = "Unknown Build Type";
        std::string graphicsAPITypeString = "Unknown Graphics API";
        pxl::WindowMode windowMode = pxl::WindowMode::Windowed;

#ifdef TA_DEBUG
        buildTypeString = "Debug x64";
#elif TA_RELEASE
        buildTypeString = "Release x64";
#elif TA_DIST
        buildTypeString = "Distribute x64";
#endif
        // TODO: unhardcode
        graphicsAPITypeString = pxl::Utils::ToString(pxl::GraphicsAPIType::Vulkan);
        std::string windowTitle = std::format("pxlFramework Test App - {} - {} - Running Test '{}'", buildTypeString, graphicsAPITypeString, m_Test->ToString());

        pxl::WindowSpecs windowSpecs = {};
        windowSpecs.Size = { 1280, 720 };
        windowSpecs.Title = windowTitle;
        windowSpecs.WindowMode = windowMode;
        windowSpecs.IconPath = "assets/pxl.png";

        OverrideWithFrameworkIni(windowSpecs);

        return windowSpecs;
    }

    void TestApplication::OnKeyDownEvent(const pxl::KeyDownEvent& e)
    {
        if (e.IsKey(pxl::KeyCode::Tab))
            m_ShowMainMenu = !m_ShowMainMenu;

        if (e.IsModsAndKey(pxl::KeyMod::Shift, pxl::KeyCode::Tab))
            m_ShowImGui = !m_ShowImGui;
    }
}