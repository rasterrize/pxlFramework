#include "TestApplication.h"

// Tests
#include "Tests/AudioTest.h"
#include "Tests/CubesTest.h"
#include "Tests/EmptyApp.h"
#include "Tests/LinesTest.h"
#include "Tests/ModelViewer.h"
#include "Tests/OGLVK.h"
#include "Tests/QuadsTest.h"
#include "Tests/WindowTest.h"

#define STRR(X) #X
#define STR(X) STRR(X)

#define TEST_NAME ModelViewer
#define TEST_NAME_STRING STR(TEST_NAME)

namespace TestApp
{
    TestApplication::TestApplication()
    {
        auto frameworkSettings = pxl::FrameworkConfig::GetSettings();

        std::string windowTitle = "pxlFramework Test App";
        std::string buildType = "Unknown Build Type";
        std::string rendererAPIType = "Unknown Renderer API";
        pxl::RendererAPIType windowRendererAPI = frameworkSettings.RendererAPI;
        pxl::WindowMode windowMode = frameworkSettings.WindowMode;

#ifdef TA_DEBUG
        buildType = "Debug x64";
#elif TA_RELEASE
        buildType = "Release x64";
#elif TA_DIST
        buildType = "Distribute x64";
#endif

        rendererAPIType = pxl::EnumStringHelper::ToString(windowRendererAPI);

        windowTitle = "pxlFramework Test App - " + buildType + " - " + rendererAPIType + " - Running Test '" TEST_NAME_STRING "'";

        pxl::WindowSpecs windowSpecs = {};
        windowSpecs.Size = frameworkSettings.WindowSize;
        windowSpecs.Position = frameworkSettings.WindowPosition;
        windowSpecs.Title = windowTitle;
        windowSpecs.RendererAPI = windowRendererAPI;
        windowSpecs.WindowMode = windowMode;
        windowSpecs.MonitorIndex = frameworkSettings.MonitorIndex;
        windowSpecs.IconPath = "assets/pxl.png";

        m_OnStartFunc = TEST_NAME::OnStart;
        m_OnUpdateFunc = TEST_NAME::OnUpdate;
        m_OnRenderFunc = TEST_NAME::OnRender;
        m_OnGUIRenderFunc = TEST_NAME::OnGUIRender;
        m_OnCloseFunc = TEST_NAME::OnClose;
        m_GetWindowFunc = TEST_NAME::GetWindow;

        m_OnStartFunc(windowSpecs);
    }

    void TestApplication::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        m_OnUpdateFunc(dt);
    }

    void TestApplication::OnRender()
    {
        PXL_PROFILE_SCOPE;

        m_OnRenderFunc();
    }

    void TestApplication::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        auto rendererStats = pxl::Renderer::GetStats();
        ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(250.0f, 300.0f), ImGuiCond_Once);
        ImGui::Begin("TestApp Renderer Stats");

        static float elapsed = 0.0f;
        static float fps = 0.0f;
        if (elapsed > 30.0f)
        {
            fps = rendererStats.FPS;
            elapsed = 0.0f;
        }

        elapsed += rendererStats.FrameTime;

        ImGui::Text("FPS: %.0f", std::round(fps));
        ImGui::Text("Frame Time (MS): %.3f", rendererStats.FrameTime);
        ImGui::Text("Draw Calls: %u", rendererStats.DrawCalls);
        ImGui::Text("Texture Binds: %u", rendererStats.TextureBinds);
        ImGui::Text("Total Triangle Count: %u", rendererStats.GetTotalTriangleCount());
        ImGui::Text("Total Vertex Count: %u", rendererStats.GetTotalVertexCount());
        ImGui::Text("Total Index Count: %u", rendererStats.GetTotalIndexCount());

        static bool enableVSync = pxl::Renderer::GetGraphicsContext()->GetVSync();
        if (ImGui::Checkbox("Enable VSync", &enableVSync))
            pxl::Renderer::GetGraphicsContext()->SetVSync(enableVSync);

        static bool limitFPS = GetFramerateMode() != pxl::FramerateMode::Unlimited ? true : false;
        if (ImGui::Checkbox("Enable FPS Limiter", &limitFPS))
        {
            if (limitFPS)
                SetFramerateMode(pxl::FramerateMode::Custom);
            else
                SetFramerateMode(pxl::FramerateMode::Unlimited);
        }

        static int32_t fpsLimit = GetFPSLimit();
        if (ImGui::InputInt("FPS Limit", &fpsLimit))
            SetFPSLimit(std::max(1, fpsLimit));

        fpsLimit = std::max(1, fpsLimit);

        ImGui::End();

        m_OnGUIRenderFunc();
    }

    void TestApplication::OnClose()
    {
        auto window = m_GetWindowFunc();

        if (window)
        {
            // Save framework settings
            // NOTE: Using auto here causes the settings to be stored as value
            pxl::FrameworkSettings& frameworkSettings = pxl::FrameworkConfig::GetSettings();
            frameworkSettings.RendererAPI = pxl::Renderer::GetCurrentAPI();
            frameworkSettings.WindowMode = window->GetWindowMode();
            frameworkSettings.VSync = window->GetGraphicsContext()->GetVSync();
            frameworkSettings.WindowPosition = window->GetPosition();
            frameworkSettings.WindowSize = window->GetSize();
            frameworkSettings.CustomFramerateCap = GetFPSLimit();
            frameworkSettings.MonitorIndex = window->GetCurrentMonitor().Index;
            frameworkSettings.FullscreenRefreshRate = window->GetCurrentMonitor().GetCurrentVideoMode().RefreshRate;
        }

        m_OnCloseFunc();
    }
}