#include "TestApplication.h"

namespace TestApp
{ 
    void TestApplication::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnUpdate(dt);
    }

    void TestApplication::OnRender()
    {
        PXL_PROFILE_SCOPE;

        if (m_Test)
            m_Test->OnRender();
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
            frameworkSettings.RendererAPI = pxl::Renderer::GetCurrentAPI();
            frameworkSettings.WindowMode = window->GetWindowMode();
            frameworkSettings.VSync = window->GetGraphicsContext()->GetVSync();
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
        std::string rendererAPITypeString = "Unknown Renderer API";
        pxl::RendererAPIType windowRendererAPI = frameworkSettings.RendererAPI;
        pxl::WindowMode windowMode = frameworkSettings.WindowMode;

#ifdef TA_DEBUG
        buildTypeString = "Debug x64";
#elif TA_RELEASE
        buildTypeString = "Release x64";
#elif TA_DIST
        buildTypeString = "Distribute x64";
#endif

        rendererAPITypeString = pxl::EnumStringHelper::ToString(windowRendererAPI);

        windowTitle = std::format("pxlFramework Test App - {} - {} - Running Test '{}'", buildTypeString, rendererAPITypeString, m_Test->ToString());

        pxl::WindowSpecs windowSpecs = {};
        windowSpecs.Size = frameworkSettings.WindowSize;
        windowSpecs.Position = frameworkSettings.WindowPosition;
        windowSpecs.Title = windowTitle;
        windowSpecs.RendererAPI = windowRendererAPI;
        windowSpecs.WindowMode = windowMode;
        windowSpecs.MonitorIndex = frameworkSettings.MonitorIndex;
        windowSpecs.IconPath = "assets/pxl.png";

        return windowSpecs;
    }
}