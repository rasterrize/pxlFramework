#include "CubesTest.h"

namespace TestApp
{
    static glm::vec4 s_ClearColour = glm::vec4(0.5f, 0.3f, 0.6f, 1.0f);

    static glm::vec3 s_PlayerPosition = glm::vec3(0.0f);

    static bool s_ControllingCamera = true;

    static glm::vec3 s_CubeRotation = glm::vec3(0.0f);

    void CubesTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        m_Camera = pxl::Camera::CreatePerspective({
            .FOV = 45.0f,
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = 0.1f,
            .FarClip = 1000.0f,
        });

        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetClearColour(s_ClearColour);

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::Cube, m_Camera);

        m_Window->GetGraphicsContext()->SetVSync(false);

        pxl::GUI::Init(m_Window);
    }

    void CubesTest::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        auto cameraPosition = m_Camera->GetPosition();
        auto cameraRotation = m_Camera->GetRotation();
        auto cameraFOV = m_Camera->GetFOV();
        auto cameraSpeed = 2.0f;
        auto cursorPos = pxl::Input::GetCursorPosition();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Tab))
        {
            s_ControllingCamera = !s_ControllingCamera;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftShift))
        {
            cameraSpeed *= 5.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::W))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.y += cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::A))
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
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::S))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.y -= cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::D))
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

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::Q))
        {
            cameraPosition.z -= cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::E))
        {
            cameraPosition.z += cameraSpeed * dt;
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraFOV -= cameraSpeed * 0.5f;
        }
        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraFOV += cameraSpeed * 0.5f;
        }

        m_Camera->SetPosition({ cameraPosition.x, cameraPosition.y, cameraPosition.z });
        m_Camera->SetFOV(cameraFOV);

        s_CubeRotation.x += 60.0f * dt;
        s_CubeRotation.y += 60.0f * dt;
        s_CubeRotation.z += 60.0f * dt;
    }

    void CubesTest::OnRender()
    {
        PXL_PROFILE_SCOPE;

        for (int32_t x = -5; x < 10; x += 2)
        {
            for (int32_t y = -5; y < 10; y += 2)
            {
                for (int32_t z = -5; z < 10; z += 2)
                {
                    pxl::Renderer::AddCube(glm::vec3(x, y, z), s_CubeRotation, glm::vec3(1.0f), glm::vec4(0.8f, 0.5f, 0.3f, 1.0f));
                }
            }
        }

        //pxl::Renderer::AddCube(glm::vec3(0.0f), s_CubeRotation, glm::vec3(1.0f), glm::vec4(0.8f, 0.5f, 0.3f, 1.0f));
    }

    void CubesTest::OnGUIRender()
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

            // TODO: Write this a better way
            if (monitorIndex <= 0)
                monitorIndex++;
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