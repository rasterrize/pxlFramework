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
                if (testValue == "ModelViewer")
                    LaunchTest<TestApp::ModelViewer>();
                // else if (testValue == "QuadsTest")
                //     LaunchTest<TestApp::QuadsTest>();
                // else if (testValue == "CubesTest")
                //     LaunchTest<TestApp::CubesTest>();
                // else if (testValue == "LinesTest")
                //     LaunchTest<TestApp::LinesTest>();
                else if (testValue == "EmptyApp")
                    LaunchTest<TestApp::EmptyApp>();
                else if (testValue == "MultiWindow")
                    LaunchTest<TestApp::MultiWindow>();
            }
        }

        if (!m_Test)
            LaunchTest<TestApp::TA_DEFAULT_TEST>();
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
        auto window = m_Test->GetWindow();

        if (window && false)
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

#ifdef PXL_ENABLE_IMGUI
    void TestApplication::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;
        // ImGui::ShowDemoWindow();

        auto& renderer = GetRenderer();

        static bool showApplicationStats = false;
        static bool showWindowStats = false;
        static bool showRendererStats = false;
        static bool showInputStats = false;
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Statistics"))
            {
                ImGui::MenuItem("Application statistics", nullptr, &showApplicationStats);
                ImGui::MenuItem("Window statistics", nullptr, &showWindowStats);
                ImGui::MenuItem("Renderer statistics", nullptr, &showRendererStats);
                ImGui::MenuItem("Input statistics", nullptr, &showInputStats);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::SeparatorText("Application");

                pxl::FramerateMode framerateMode = GetFramerateMode();
                auto framerateModeString = pxl::Utils::ToString(framerateMode);

                static int item_selected_idx = 0; // Here we store our selection data as an index.
                const char* combo_preview_value = framerateModeString.c_str();
                if (ImGui::BeginCombo("Framerate Mode", combo_preview_value))
                {
                    for (int n = 0; n <= static_cast<int>(pxl::FramerateMode::AdaptiveSync); n++)
                    {
                        const bool is_selected = (item_selected_idx == n);
                        auto idxFramerateMode = static_cast<pxl::FramerateMode>(n);
                        if (ImGui::Selectable(pxl::Utils::ToString(idxFramerateMode).c_str(), is_selected))
                        {
                            item_selected_idx = n;
                            pxl::Application::SetFramerateMode(idxFramerateMode);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                int fpsLimit = static_cast<int>(GetFPSLimit());

                if (GetFramerateMode() != pxl::FramerateMode::Custom)
                    ImGui::BeginDisabled();

                if (ImGui::DragInt("FPS Limit", &fpsLimit, 1.0f, 3, 1000))
                {
                    Application::Get().SetFPSLimit(fpsLimit);
                }

                if (GetFramerateMode() != pxl::FramerateMode::Custom)
                {
                    ImGui::SetItemTooltip("Set framerate mode to custom to enable this setting");
                    ImGui::EndDisabled();
                }

                ImGui::SeparatorText("Renderer");

                bool vsync = renderer.GetConfig().VerticalSync;
                if (ImGui::Checkbox("Vertical Sync", &vsync))
                {
                    renderer.SetVerticalSync(vsync);
                }

                ImGui::BeginDisabled(vsync);

                bool allowTearing = renderer.GetConfig().AllowTearing;
                if (ImGui::Checkbox("Allow Tearing", &allowTearing))
                {
                    renderer.SetAllowTearing(allowTearing);
                }

                ImGui::EndDisabled();

                if (renderer.GetGraphicsDevice().GetLimits().ForcedTripleBuffering)
                    ImGui::BeginDisabled();

                bool tripleBuffering = renderer.GetConfig().TripleBuffering;
                if (ImGui::Checkbox("Triple Buffering", &tripleBuffering))
                {
                    renderer.SetAllowTearing(tripleBuffering);
                }
                ImGui::SetItemTooltip("Sets the swapchains desired image count to 3 if supported");

                if (renderer.GetGraphicsDevice().GetLimits().ForcedTripleBuffering)
                {
                    ImGui::SetItemTooltip("Triple buffering is forced by device");
                    ImGui::EndDisabled();
                }

                glm::vec4 clearColour = renderer.GetConfig().ClearColour;
                if (ImGui::ColorEdit4("Clear Colour", &clearColour.x))
                {
                    renderer.SetClearColour(clearColour);
                }

                if (ImGui::Button("Reload pipelines"))
                {
                    renderer.ReloadPipelines();
                }

                if (ImGui::Button("Clear shader cache"))
                {
                    renderer.ClearShaderCache();
                }

                ImGui::SeparatorText("Window");

                auto window = m_Test->GetWindow();

                pxl::WindowMode windowMode = window->GetWindowMode();
                std::string windowModeString = pxl::Utils::ToString(windowMode);

                if (ImGui::BeginCombo("Window Mode", windowModeString.c_str()))
                {
                    for (int n = 0; n <= static_cast<int>(pxl::WindowMode::Fullscreen); n++)
                    {
                        auto idxWindowMode = static_cast<pxl::WindowMode>(n);
                        const bool is_selected = (windowMode == idxWindowMode);
                        if (ImGui::Selectable(pxl::Utils::ToString(idxWindowMode).c_str(), is_selected))
                        {
                            window->SetWindowMode(idxWindowMode);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                auto videoModes = window->GetCurrentMonitor().VideoModes;
                auto currentVideoMode = window->GetCurrentMonitor().GetCurrentVideoMode();
                if (ImGui::BeginCombo("Video mode", currentVideoMode.ToString().c_str()))
                {
                    for (size_t n = 0; n < videoModes.size(); n++)
                    {
                        // auto idxWindowMode = static_cast<pxl::WindowMode>(n);
                        // const bool is_selected = (windowMode == idxWindowMode);
                        if (ImGui::Selectable(videoModes.at(n).ToString().c_str(), false))
                        {
                            // window->SetWindowMode(idxWindowMode);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        // if (is_selected)
                        //     ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (showApplicationStats)
        {
            ImGui::Begin("Application Stats", &showApplicationStats);
            ImGui::End();
        }

        if (showWindowStats)
        {
            ImGui::Begin("Window Stats", &showWindowStats);

            auto window = m_Test->GetWindow();
            ImGui::Text("Window Title: %s", window->GetTitle().c_str());
            ImGui::Text("Window Position: %d, %d", window->GetPosition().x, window->GetPosition().y);
            ImGui::Text("Window Size: %u x %u", window->GetSize().Width, window->GetSize().Height);

            ImGui::End();
        }

        if (showRendererStats)
        {
            ImGui::Begin("Renderer Stats", &showRendererStats);

            // auto& stats = m_RendererStats;
            // ImGui::SeparatorText("Renderer");
            // ImGui::Text("FPS: %.3f", renderer.GetFramesPerSecond());
            // ImGui::Text("Frame Time: %.3f", stats.PreviousFrameTime);
            // ImGui::Text("Frame Count: %u", stats.FrameCount);

            // ImGui::SeparatorText("Frame");
            // // ImGui::Text("Draw calls: %u", stats.GraphicsContextStats.DrawCalls);
            // // ImGui::Text("Vertex count: %u", stats.FrameStats.VertexCount);
            // // ImGui::Text("Index count: %u", stats.FrameStats.IndexCount);
            // // ImGui::Text("Triangle count: %u", stats.FrameStats.GetTriangleCount());
            // // ImGui::Text("Pipeline binds: %u", stats.GraphicsContextStats.PipelineBinds);
            // // ImGui::Text("Texture binds: %u", stats.FrameStats.TextureBinds);

            auto gpuName = renderer.GetGraphicsDevice().GetGPUName();
            auto driverVersion = renderer.GetGraphicsDevice().GetDriverInfo();
            ImGui::SeparatorText("Graphics Device");
            ImGui::Text("GPU name: %s", gpuName.c_str());
            ImGui::Text("Driver version: %s", driverVersion.c_str());
            // ImGui::Text("Buffer count: %u", gpuStats.BufferCount);
            // ImGui::Text("Shader count: %u", gpuStats.ShaderCount);
            // ImGui::Text("Graphics pipeline count: %u", gpuStats.GraphicsPipelineCount);
            // ImGui::Text("Texture count: %u", gpuStats.TextureCount);
            ImGui::End();
        }

        if (showInputStats)
        {
            ImGui::Begin("Input Stats", &showInputStats);
            ImGui::End();
        }

        if (m_Test)
            m_Test->OnGUIRender();
    }
#endif

    pxl::WindowSpecs TestApplication::CreateWindowSpecs()
    {
        pxl::FrameworkSettings frameworkSettings = {};

        std::string buildTypeString = "Unknown Build Type";
        std::string graphicsAPITypeString = "Unknown Graphics API";
        pxl::WindowMode windowMode = frameworkSettings.WindowMode;

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
        windowSpecs.Size = frameworkSettings.WindowSize;
        windowSpecs.Position = frameworkSettings.WindowPosition;
        windowSpecs.Title = windowTitle;
        windowSpecs.WindowMode = windowMode;
        windowSpecs.MonitorIndex = frameworkSettings.MonitorIndex;
        windowSpecs.IconPath = "assets/pxl.png";

        return windowSpecs;
    }
}