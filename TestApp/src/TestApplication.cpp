#include "TestApplication.h"

// Tests
#include "Tests/CubesTest.h"
#include "Tests/EmptyApp.h"
#include "Tests/LinesTest.h"
#include "Tests/ModelViewer.h"
#include "Tests/OGLVK.h"
#include "Tests/QuadsTest.h"
#include "Tests/WindowTest.h"

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

        windowTitle = "pxlFramework Test App - " + buildType + " - " + rendererAPIType;

        pxl::WindowSpecs windowSpecs = {};
        windowSpecs.Size = { 1600, 900 };
        windowSpecs.Title = windowTitle;
        windowSpecs.RendererAPI = windowRendererAPI;
        windowSpecs.WindowMode = windowMode;
        windowSpecs.IconPath = "assets/pxl.png";

#define TEST_NAME QuadsTest

        m_OnStartFunc = TEST_NAME::OnStart;
        m_OnUpdateFunc = TEST_NAME::OnUpdate;
        m_OnRenderFunc = TEST_NAME::OnRender;
        m_OnGuiRender = TEST_NAME::OnGUIRender;

        m_OnStartFunc(windowSpecs);
    }

    TestApplication::~TestApplication()
    {
#if SAVEFRAMEWORKSETTINGS
        // Save framework settings // TODO: this should be automatically handled by the application class
        auto frameworkSettings = pxl::FrameworkConfig::GetSettings();

        frameworkSettings.WindowMode = pxl::WindowMode::Windowed;
        frameworkSettings.RendererAPI = pxl::Renderer::GetCurrentAPI();

        pxl::FrameworkConfig::SetSettings(frameworkSettings);
#endif
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
        ImGui::SetNextWindowSize(ImVec2(200.0f, 200.0f));
        ImGui::Begin("TestApp Renderer Stats");

        ImGui::Text("FPS: %.2f", pxl::Renderer::GetFPS());
        ImGui::Text("Frame Time (MS): %.3f", pxl::Renderer::GetFrameTimeMS());
        ImGui::Text("Draw Calls: %u", rendererStats.DrawCalls);
        ImGui::Text("Texture Binds: %u", rendererStats.TextureBinds);
        ImGui::Text("Total Triangle Count: %u", rendererStats.GetTotalTriangleCount());
        ImGui::Text("Total Vertex Count: %u", rendererStats.GetTotalVertexCount());
        ImGui::Text("Total Index Count: %u", rendererStats.GetTotalIndexCount());

        ImGui::End();

        m_OnGuiRender();
    }
}