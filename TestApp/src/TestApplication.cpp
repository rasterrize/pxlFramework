#include "TestApplication.h"

namespace TestApp
{
    void TestApplication::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnUpdate(dt);
    }

    void TestApplication::OnRender(const std::unique_ptr<pxl::Renderer>& renderer)
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnRender(renderer);
    }

    void TestApplication::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnGUIRender();
    }

    void TestApplication::OnClose()
    {
        auto window = m_Test->GetWindow();

        if (window)
        {
            // Save framework settings
            // NOTE: Using auto here causes the settings to be stored as value
            pxl::FrameworkSettings& frameworkSettings = pxl::FrameworkConfig::GetSettings();
            frameworkSettings.WindowMode = window->GetWindowMode();
            frameworkSettings.WindowPosition = window->GetPosition();
            frameworkSettings.WindowSize = window->GetSize();
            frameworkSettings.CustomFramerateCap = GetFPSLimit();
            frameworkSettings.MonitorIndex = window->GetCurrentMonitor().Index;
            frameworkSettings.FullscreenRefreshRate = window->GetCurrentMonitor().GetCurrentVideoMode().RefreshRate;
        }

        if (m_Test)
            m_Test->OnClose();
    }

    pxl::WindowSpecs TestApplication::CreateWindowSpecs()
    {
        auto frameworkSettings = pxl::FrameworkConfig::GetSettings();

        std::string windowTitle = "pxlFramework Test App";
        std::string buildTypeString = "Unknown Build Type";
        std::string graphicsAPITypeString = "Unknown Renderer API";
        pxl::WindowMode windowMode = frameworkSettings.WindowMode;

#ifdef TA_DEBUG
        buildTypeString = "Debug x64";
#elif TA_RELEASE
        buildTypeString = "Release x64";
#elif TA_DIST
        buildTypeString = "Distribute x64";
#endif

        windowTitle = std::format("pxlFramework Test App - {} - {} - Running Test '{}'", buildTypeString, graphicsAPITypeString, m_Test->ToString());

        pxl::WindowSpecs windowSpecs = {};
        windowSpecs.Size = frameworkSettings.WindowSize;
        windowSpecs.Position = frameworkSettings.WindowPosition;
        windowSpecs.Title = windowTitle;
        windowSpecs.WindowMode = windowMode;
        windowSpecs.MonitorIndex = frameworkSettings.MonitorIndex;
        windowSpecs.IconPath = "assets/pxl.png";

        return windowSpecs;
    }
}