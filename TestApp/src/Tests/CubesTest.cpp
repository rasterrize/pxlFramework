#include "CubesTest.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_Window = nullptr;
    static std::shared_ptr<pxl::PerspectiveCamera> s_Camera = nullptr;

    static glm::vec4 s_ClearColour = glm::vec4(glm::vec3(0.0f), 1.0f);

    static glm::vec3 s_PlayerPosition = glm::vec3(0.0f);

    static bool s_ControllingCamera = true;

    void CubesTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        s_Window = pxl::Window::Create(windowSpecs);
        
        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);

        s_Camera = pxl::PerspectiveCamera::Create({
            .FOV = 45.0f,
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = -10.0f,
            .FarClip = 10.0f,
        });

        s_Camera->SetPosition({0.0f, 0.0f, 0.0f});

        pxl::Renderer::SetClearColour(s_ClearColour);
    }

    void CubesTest::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;
        
        auto cameraPosition = s_Camera->GetPosition();
        auto cameraRotation = s_Camera->GetRotation();
        auto cameraFOV = s_Camera->GetFOV();
        auto cameraSpeed = 2.0f;
        auto cursorPos = pxl::Input::GetCursorPosition();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_TAB))
        {
            if (s_ControllingCamera)
                s_ControllingCamera = false;
            else
                s_ControllingCamera = true;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 5.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.y += cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.x -= cameraSpeed * dt;
            }
            else
            {
                s_PlayerPosition.x -= 1.0f * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.y -= cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.x += cameraSpeed * dt;
            }
            else
            {
                s_PlayerPosition.x += 1.0f * dt;
            }
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraFOV -= cameraSpeed * 0.5f;
        }
        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraFOV += cameraSpeed * 0.5f;
        }

        s_Camera->SetPosition({cameraPosition.x, cameraPosition.y, cameraPosition.z});
        s_Camera->SetFOV(cameraFOV);
    }

    void CubesTest::OnRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void CubesTest::OnClose()
    {
    }

    void CubesTest::OnGuiRender()
    {
        PXL_PROFILE_SCOPE;
        
    #if IMGUICUBESTEST
        ImGui::ShowDemoWindow();

        // Main debug window
        {
            ImGui::SetNextWindowSize(ImVec2(300, 680), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(21, 21), ImGuiCond_FirstUseEver);

            ImGui::Begin("pxlFramework: Test App");

            ImGui::Text("FPS: %.2f (%.3fms)", pxl::Renderer::GetFPS(), pxl::Renderer::GetFrameTimeMS());
            ImGui::Text("Clear Colour:");
            if (ImGui::ColorEdit3("Clear Colour", &m_ClearColour.x))
                pxl::Renderer::SetClearColour(m_ClearColour);
            ImGui::Text("Quad Colour:");
            ImGui::ColorEdit3("Quad Colour", &m_QuadColour.x);
            if (ImGui::Button("Reload Shader"))
            {
                m_Shader->Reload();
            }
            ImGui::Text("Camera FOV: %.2f", m_Camera->GetFOV());
            //ImGui::Text("Camera Position: %.3f, %.3f, %.3f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
            //ImGui::Text("Camera Rotation: %.3f, %.3f, %.3f", m_CameraRotation.x, m_CameraRotation.y, m_CameraRotation.z);

            auto stats = pxl::Renderer::GetStats();

            ImGui::Text("Renderer Stats:");
            ImGui::Text("- Draw Calls: %u", stats.DrawCalls);
            ImGui::Text("- Vertices: %u", stats.QuadVertexCount + stats.LineVertexCount);
            ImGui::Text("- Indices: %u", stats.QuadIndexCount);
            ImGui::Text("- Triangles: %u", stats.GetTriangleCount());

            auto cursorPos = pxl::Input::GetCursorPosition(); 

            ImGui::Text("Cursor Pos: %f, %f", cursorPos.x, cursorPos.y);

            static int i0 = 100;
            ImGui::InputInt("Blue Quad Amount", &i0);
            m_BlueQuadAmount = i0;

            static int i1 = 100;
            ImGui::InputInt("Orange Quad Amount", &i1);
            m_OrangeQuadAmount = i1;

            ImGui::End();
        }

        // Settings Window
        {
            ImGui::SetNextWindowSize(ImVec2(330, 200), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(929, 21), ImGuiCond_FirstUseEver);

            ImGui::Begin("Settings");

            // Monitor select
            static int monitorIndex = 1;
            int step = 1;
            int monitorCount = pxl::Window::GetMonitorCount();
            ImGui::SliderInt("Monitor", &monitorIndex, 1, monitorCount);
            if (monitorIndex <= 0)
                monitorIndex++; // TODO: turn this into lambda function
            if (monitorIndex > monitorCount)
                monitorIndex--;

            // Display mode
            const char* windowModes[] = { "Windowed", "Borderless", "Fullscreen" };
            static int windowModeInput = 0;
            static pxl::WindowMode windowMode;
            static pxl::WindowMode windowModeToSet;

            if (windowMode != s_Window->GetWindowMode())
            {
                switch (s_Window->GetWindowMode())
                {
                    case pxl::WindowMode::Windowed:
                        windowModeInput = 0;
                        break;
                    case pxl::WindowMode::Borderless:
                        windowModeInput = 1;
                        break;
                    case pxl::WindowMode::Fullscreen:
                        windowModeInput = 2;
                        break;
                }

                windowMode = s_Window->GetWindowMode();
            }

            ImGui::Combo("Display Mode", &windowModeInput, windowModes, IM_ARRAYSIZE(windowModes));

            switch (windowModeInput)
            {
                case 0:
                    windowModeToSet = pxl::WindowMode::Windowed;
                    break;
                case 1:
                    windowModeToSet = pxl::WindowMode::Borderless;
                    break;
                case 2:
                    windowModeToSet = pxl::WindowMode::Fullscreen;
                    break;
            }

            // VSync
            static bool vsync = s_Window->GetGraphicsContext()->GetVSync();
            ImGui::Checkbox("VSync", &vsync);

            // Camera FOV
            static float cameraFOVInput = m_Camera->GetFOV(); // should retrieve cameras FOV
            static float cameraFOV = m_Camera->GetFOV();

            ImGui::SliderFloat("Camera FOV", &cameraFOVInput, 30.0f, 120.0f);

            // Apply Button
            if (ImGui::Button("Apply"))
            {
                s_Window->SetVSync(vsync);
                s_Window->SetMonitor(monitorIndex);
                s_Window->SetWindowMode(windowModeToSet);

                cameraFOV = cameraFOVInput;
            }

            ImGui::End();
        }

        // Audio player window
        ImGui::SetNextWindowSize(ImVec2(330, 400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(929, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Audio Player");

        const char* tracks[20]; // should probably set a const max here

        for (int i = 0; i < m_AudioLibrary.size(); i++)
        {
            tracks[i] = m_AudioLibrary[i].c_str();
        }

        static int selectedAudioIndex;
        static std::string selectedAudioName = m_AudioLibrary[0];

        ImGui::Text("Audio Library");
        static int item_current_idx = 0; // Here we store our selection data as an index.
        if (ImGui::BeginListBox("##"))
        {
            for (int n = 0; n < m_AudioLibrary.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(m_AudioLibrary[n].c_str(), is_selected))
                {
                    item_current_idx = n;
                    selectedAudioName = m_AudioLibrary[n];
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
        ImGui::EndListBox();
        }
            
        if (ImGui::Button("Play"))
        {
            pxl::AudioManager::Play(selectedAudioName);
        }

        if (ImGui::Button("Pause"))
        {
            pxl::AudioManager::Pause(selectedAudioName);
        }

        if (ImGui::Button("Stop"))
        {
            pxl::AudioManager::Stop(selectedAudioName);
        }

        ImGui::End();
    #endif
    }
}