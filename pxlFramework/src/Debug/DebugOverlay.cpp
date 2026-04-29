#include "DebugOverlay.h"

#include <imgui.h>

namespace pxl
{
    void DebugOverlay::Render(Window& window, Renderer& renderer)
    {
        if (!m_Show)
            return;

        static bool showApplicationStats = false;
        static bool showWindowStats = false;
        static bool showRendererStats = false;
        static bool showInputStats = false;
        static bool showMainMenu = true;

        if (showMainMenu)
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::Button("Hide"))
                    m_Show = false;

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

                    FramerateMode framerateMode = renderer.GetConfig().FramerateMode;
                    auto framerateModeString = Utils::ToString(framerateMode);

                    static int item_selected_idx = 0; // Here we store our selection data as an index.
                    const char* combo_preview_value = framerateModeString.c_str();
                    if (ImGui::BeginCombo("Framerate Mode", combo_preview_value))
                    {
                        for (int n = 0; n <= static_cast<int>(FramerateMode::AdaptiveSync); n++)
                        {
                            const bool is_selected = (item_selected_idx == n);
                            auto idxFramerateMode = static_cast<FramerateMode>(n);
                            if (ImGui::Selectable(Utils::ToString(idxFramerateMode).c_str(), is_selected))
                            {
                                item_selected_idx = n;
                                renderer.SetFramerateMode(idxFramerateMode);
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    int fpsLimit = static_cast<int>(renderer.GetConfig().CustomFramerateLimit);

                    if (framerateMode != FramerateMode::Custom)
                        ImGui::BeginDisabled();

                    if (ImGui::DragInt("FPS Limit", &fpsLimit, 1.0f, 10, 1000, "%d", ImGuiSliderFlags_ClampOnInput))
                    {
                        renderer.SetCustomFramerateLimit(fpsLimit);
                    }

                    if (framerateMode != FramerateMode::Custom)
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
                        renderer.SetTripleBuffering(tripleBuffering);
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

                    WindowMode windowMode = window.GetWindowMode();
                    std::string windowModeString = Utils::ToString(windowMode);

                    if (ImGui::BeginCombo("Window Mode", windowModeString.c_str()))
                    {
                        for (int n = 0; n <= static_cast<int>(WindowMode::Fullscreen); n++)
                        {
                            auto idxWindowMode = static_cast<WindowMode>(n);
                            const bool is_selected = (windowMode == idxWindowMode);
                            if (ImGui::Selectable(Utils::ToString(idxWindowMode).c_str(), is_selected))
                            {
                                window.SetWindowMode(idxWindowMode);
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    auto videoModes = window.GetCurrentMonitor().VideoModes;
                    auto currentVideoMode = window.GetCurrentMonitor().GetCurrentVideoMode();
                    if (ImGui::BeginCombo("Video mode", currentVideoMode.ToString().c_str()))
                    {
                        for (size_t n = 0; n < videoModes.size(); n++)
                        {
                            // auto idxWindowMode = static_cast<WindowMode>(n);
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
        }

        if (showApplicationStats)
        {
            ImGui::Begin("Application Stats", &showApplicationStats);
            ImGui::End();
        }

        if (showWindowStats)
        {
            ImGui::Begin("Window Stats", &showWindowStats);

            ImGui::Text("Window Title: %s", window.GetTitle().data());
            ImGui::Text("Window Position: %d, %d", window.GetPosition().x, window.GetPosition().y);
            ImGui::Text("Window Size: %u x %u", window.GetSize().Width, window.GetSize().Height);

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

            auto& stats = renderer.GetFrameStats();

            static Stopwatch statsSW;
            static Renderer::FrameStatistics slowStats = renderer.GetFrameStats();
            if (statsSW.GetElapsedSec() > 0.05)
            {
                slowStats = renderer.GetFrameStats();
                statsSW.Reset();
            }

            static Stopwatch fpsSW;
            static double slowFPS = std::round(slowStats.GetFPS());
            if (fpsSW.GetElapsedSec() > 1.0)
            {
                slowFPS = std::round(slowStats.GetFPS());
                fpsSW.Reset();
            }

            const size_t sampleCount = 50;
            static std::vector<float> frameTimes;

            frameTimes.push_back(static_cast<float>(stats.FrameTime));
            if (frameTimes.size() == sampleCount)
                frameTimes.erase(std::find(frameTimes.begin(), frameTimes.end(), frameTimes.front()));

            ImGui::SeparatorText("Frame");
            ImGui::Text("%.0f FPS", slowFPS);
            ImGui::Text("%.3f FPS Raw", slowStats.GetFPS());
            ImGui::Text("Frame Count: %u", stats.FrameCountIndex);
            ImGui::Text("Frame Time: %.3fms", slowStats.FrameTime);
            ImGui::PlotLines("Frame Graph", frameTimes.data(), static_cast<int32_t>(frameTimes.size()), sampleCount, "", FLT_MIN, FLT_MAX, ImVec2(0, 50));
            ImGui::Text("Render Time: %.3fms", slowStats.RenderTime);
            ImGui::Text("Framerate Limit Wait Time: %.3fms", slowStats.FramerateLimitWaitTime);
            ImGui::Text("Framerate Limit Sleep Time: %.3fms", slowStats.FramerateLimitSleepTime);
            ImGui::Text("Framerate Limit Spin Time: %.3fms", slowStats.FramerateLimitSpinTime);
            ImGui::Text("Graphics Device Wait Time: %.3fms", slowStats.GraphicsDeviceWaitTime);
            ImGui::Text("Frame In Flight Index: %u", stats.FrameInFlightIndex);

            // ImGui::Text("Draw calls: %u", stats.GraphicsContextStats.DrawCalls);
            // ImGui::Text("Vertex count: %u", stats.FrameStats.VertexCount);
            // ImGui::Text("Index count: %u", stats.FrameStats.IndexCount);
            // ImGui::Text("Triangle count: %u", stats.FrameStats.GetTriangleCount());
            // ImGui::Text("Pipeline binds: %u", stats.GraphicsContextStats.PipelineBinds);
            // ImGui::Text("Texture binds: %u", stats.FrameStats.TextureBinds);

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
    }
}