#include "TestApplication.h"

// Tests
#include "Tests/CubesTest.h"
#include "Tests/EmptyApp.h"
#include "Tests/LinesTest.h"
#include "Tests/ModelViewer.h"
#include "Tests/OGLVK.h"
#include "Tests/QuadsTest.h"
#include "Tests/WindowTest.h"

#define TEST_NAME ModelViewer
#define TEST_NAME_STRING "ModelViewer" // TODO: If its possible, set this automatically using TEST_NAME

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
        windowSpecs.Size = { 1600, 900 };
        windowSpecs.Title = windowTitle;
        windowSpecs.RendererAPI = windowRendererAPI;
        windowSpecs.WindowMode = windowMode;
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

    void TestApplication::OnGUIRender() // Function only gets called if ImGui is initialized
    {
        PXL_PROFILE_SCOPE;

        auto rendererStats = pxl::Renderer::GetStats();
        //ImGui::SetNextWindowSize(ImVec2(200.0f, 200.0f));
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

        if (ImGui::Button("Toggle VSync"))
            pxl::Renderer::GetGraphicsContext()->ToggleVSync();

        static int32_t fpsLimit = 0;
        ImGui::InputInt("FPS Limit", &fpsLimit);

        if (ImGui::Button("Set FPS Limit"))
            SetFPSLimit(fpsLimit);

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
        }

        m_OnCloseFunc();
    }
}