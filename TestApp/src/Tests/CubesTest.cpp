#include "CubesTest.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> CubesTest::m_Window;
    std::shared_ptr<pxl::Camera> CubesTest::m_Camera;
    std::shared_ptr<pxl::Shader> CubesTest::m_Shader; 
    std::shared_ptr<pxl::GraphicsPipeline> CubesTest::m_Pipeline;
    glm::vec4 CubesTest::m_ClearColour;

    glm::vec3 CubesTest::m_CameraPosition = glm::vec3(0.0f);
    glm::vec3 CubesTest::m_CameraRotation = glm::vec3(0.0f);
    glm::vec3 CubesTest::m_PlayerPosition = { 0.0f, 0.75f, 2.0f };
    bool CubesTest::controllingCamera = true;

    void CubesTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Orthographic, 16.0f / 9.0f, -10.0, 10.0f });
        m_Camera->SetPosition({0.0f, 0.0f, 0.0f});

        m_ClearColour = glm::vec4(0.078f, 0.094f, 0.109f, 1.0f);
        pxl::Renderer::SetClearColour(m_ClearColour);
    }

    void CubesTest::OnUpdate(float dt)
    {
        m_CameraPosition = m_Camera->GetPosition();
        m_CameraRotation = m_Camera->GetRotation();
        auto cameraSpeed = 2.0f;
        auto cameraZoom = m_Camera->GetZoom();
        auto cursorPos = pxl::Input::GetCursorPosition();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_TAB))
        {
            if (controllingCamera)
                controllingCamera = false;
            else
                controllingCamera = true;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 5.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {
            if (controllingCamera)
            {
                m_CameraPosition.y += cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        {
            if (controllingCamera)
            {
                m_CameraPosition.x -= cameraSpeed * dt;
            }
            else
            {
                m_PlayerPosition.x -= 1.0f * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            if (controllingCamera)
            {
                m_CameraPosition.y -= cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        {
            if (controllingCamera)
            {
                m_CameraPosition.x += cameraSpeed * dt;
            }
            else
            {
                m_PlayerPosition.x += 1.0f * dt;
            }
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraZoom -= cameraSpeed * 0.5f;
        }
        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraZoom += cameraSpeed * 0.5f;
        }

        m_Camera->SetPosition({m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z});
        m_Camera->SetZoom(cameraZoom);
    }

    void CubesTest::OnRender()
    {
    }

    void CubesTest::OnClose()
    {
    }

    void CubesTest::OnImGuiRender()
    {
        // ImGui::ShowDemoWindow();

        // // Main debug window
        // {
        //     ImGui::SetNextWindowSize(ImVec2(300, 680), ImGuiCond_FirstUseEver);
        //     ImGui::SetNextWindowPos(ImVec2(21, 21), ImGuiCond_FirstUseEver);

        //     ImGui::Begin("pxlFramework: Test App");

        //     ImGui::Text("FPS: %.2f (%.3fms)", pxl::Renderer::GetFPS(), pxl::Renderer::GetFrameTimeMS());
        //     ImGui::Text("Clear Colour:");
        //     if (ImGui::ColorEdit3("Clear Colour", &m_ClearColour.x))
        //         pxl::Renderer::SetClearColour(m_ClearColour);
        //     ImGui::Text("Quad Colour:");
        //     ImGui::ColorEdit3("Quad Colour", &m_QuadColour.x);
        //     if (ImGui::Button("Reload Shader"))
        //     {
        //         m_Shader->Reload();
        //     }
        //     ImGui::Text("Camera FOV: %.2f", m_Camera->GetFOV());
        //     //ImGui::Text("Camera Position: %.3f, %.3f, %.3f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
        //     //ImGui::Text("Camera Rotation: %.3f, %.3f, %.3f", m_CameraRotation.x, m_CameraRotation.y, m_CameraRotation.z);

        //     auto stats = pxl::Renderer::GetStats();

        //     ImGui::Text("Renderer Stats:");
        //     ImGui::Text("- Draw Calls: %u", stats.DrawCalls);
        //     ImGui::Text("- Vertices: %u", stats.QuadVertexCount + stats.LineVertexCount);
        //     ImGui::Text("- Indices: %u", stats.QuadIndexCount);
        //     ImGui::Text("- Triangles: %u", stats.GetTriangleCount());

        //     auto cursorPos = pxl::Input::GetCursorPosition(); 

        //     ImGui::Text("Cursor Pos: %f, %f", cursorPos.x, cursorPos.y);

        //     static int i0 = 100;
        //     ImGui::InputInt("Blue Quad Amount", &i0);
        //     m_BlueQuadAmount = i0;

        //     static int i1 = 100;
        //     ImGui::InputInt("Orange Quad Amount", &i1);
        //     m_OrangeQuadAmount = i1;

        //     ImGui::End();
        // }

        // // Settings Window
        // {
        //     ImGui::SetNextWindowSize(ImVec2(330, 200), ImGuiCond_FirstUseEver);
        //     ImGui::SetNextWindowPos(ImVec2(929, 21), ImGuiCond_FirstUseEver);

        //     ImGui::Begin("Settings");

        //     // Monitor select
        //     static int monitorIndex = 1;
        //     int step = 1;
        //     int monitorCount = pxl::Window::GetMonitorCount();
        //     ImGui::SliderInt("Monitor", &monitorIndex, 1, monitorCount);
        //     if (monitorIndex <= 0)
        //         monitorIndex++; // TODO: turn this into lambda function
        //     if (monitorIndex > monitorCount)
        //         monitorIndex--;

        //     // Display mode
        //     const char* windowModes[] = { "Windowed", "Borderless", "Fullscreen" };
        //     static int windowModeInput = 0;
        //     static pxl::WindowMode windowMode;
        //     static pxl::WindowMode windowModeToSet;

        //     if (windowMode != m_Window->GetWindowMode())
        //     {
        //         switch (m_Window->GetWindowMode())
        //         {
        //             case pxl::WindowMode::Windowed:
        //                 windowModeInput = 0;
        //                 break;
        //             case pxl::WindowMode::Borderless:
        //                 windowModeInput = 1;
        //                 break;
        //             case pxl::WindowMode::Fullscreen:
        //                 windowModeInput = 2;
        //                 break;
        //         }

        //         windowMode = m_Window->GetWindowMode();
        //     }

        //     ImGui::Combo("Display Mode", &windowModeInput, windowModes, IM_ARRAYSIZE(windowModes));

        //     switch (windowModeInput)
        //     {
        //         case 0:
        //             windowModeToSet = pxl::WindowMode::Windowed;
        //             break;
        //         case 1:
        //             windowModeToSet = pxl::WindowMode::Borderless;
        //             break;
        //         case 2:
        //             windowModeToSet = pxl::WindowMode::Fullscreen;
        //             break;
        //     }

        //     // VSync
        //     static bool vsync = m_Window->GetGraphicsContext()->GetVSync();
        //     ImGui::Checkbox("VSync", &vsync);

        //     // Camera FOV
        //     static float cameraFOVInput = m_Camera->GetFOV(); // should retrieve cameras FOV
        //     static float cameraFOV = m_Camera->GetFOV();

        //     ImGui::SliderFloat("Camera FOV", &cameraFOVInput, 30.0f, 120.0f);

        //     // Apply Button
        //     if (ImGui::Button("Apply"))
        //     {
        //         m_Window->SetVSync(vsync);
        //         m_Window->SetMonitor(monitorIndex);
        //         m_Window->SetWindowMode(windowModeToSet);

        //         cameraFOV = cameraFOVInput;
        //     }

        //     ImGui::End();
        // }

        // // Audio player window
        // ImGui::SetNextWindowSize(ImVec2(330, 400), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowPos(ImVec2(929, 300), ImGuiCond_FirstUseEver);
        // ImGui::Begin("Audio Player");

        // const char* tracks[20]; // should probably set a const max here

        // for (int i = 0; i < m_AudioLibrary.size(); i++)
        // {
        //     tracks[i] = m_AudioLibrary[i].c_str();
        // }

        // static int selectedAudioIndex;
        // static std::string selectedAudioName = m_AudioLibrary[0];

        // ImGui::Text("Audio Library");
        // static int item_current_idx = 0; // Here we store our selection data as an index.
        // if (ImGui::BeginListBox("##"))
        // {
        //     for (int n = 0; n < m_AudioLibrary.size(); n++)
        //     {
        //         const bool is_selected = (item_current_idx == n);
        //         if (ImGui::Selectable(m_AudioLibrary[n].c_str(), is_selected))
        //         {
        //             item_current_idx = n;
        //             selectedAudioName = m_AudioLibrary[n];
        //         }

        //         // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        //         if (is_selected)
        //             ImGui::SetItemDefaultFocus();
        //     }
        // ImGui::EndListBox();
        // }
            
        // if (ImGui::Button("Play"))
        // {
        //     pxl::AudioManager::Play(selectedAudioName);
        // }

        // if (ImGui::Button("Pause"))
        // {
        //     pxl::AudioManager::Pause(selectedAudioName);
        // }

        // if (ImGui::Button("Stop"))
        // {
        //     pxl::AudioManager::Stop(selectedAudioName);
        // }

        //ImGui::End();
    }
}