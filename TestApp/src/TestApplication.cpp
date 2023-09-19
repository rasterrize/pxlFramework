#include "TestApplication.h"

#include <imgui.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        std::string vertexShaderCamera = R"(
            #version 460 core

            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec2 a_TexCoords;

            out vec3 v_Position;
            out vec2 v_TexCoords;

            uniform mat4 u_VP;

            void main()
            {
                v_Position = a_Position;
                v_TexCoords = a_TexCoords;
                gl_Position = u_VP * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 460 core
            
            layout (location = 0) out vec4 color;

            in vec3 v_Position;
            in vec2 v_TexCoords;

            uniform sampler2D u_Texture;

            void main()
            {
                //color = vec4(1.0, 0.2, 0.4, 1.0);
                //color = vec4(v_Position, 1.0);
                color = texture(u_Texture, v_TexCoords);
            }
        )";

        std::string windowTitle = "pxlFramework Test App";

        #ifdef TA_DEBUG
            windowTitle = "pxlFramework Test App - Debug x64";
        #elif TA_RELEASE
            windowTitle = "pxlFramework Test App - Release x64";
        #elif TA_DIST
            windowTitle = "pxlFramework Test App - Distribute x64";
        #else
            windowTitle = "pxlFramework Test App - Unknown Build Type";
        #endif
        
        m_Window = pxl::Window::Create({1280, 720, windowTitle, pxl::RendererAPIType::OpenGL});
        m_Window->SetPosition(1920 / 2 - 1280 / 2, 1080 / 2 - 720 / 2);
        m_Window->SetVSync(true);

        pxl::Renderer::Init(m_Window);

        pxl::Input::Init(m_Window);

        pxl::AudioManager::Init(m_Window);

        auto clearColour = pxl::vec4(20.0f / 255.0f, 24.0f / 255.0f, 28.0f / 255.0f, 1.0f); // pxl::vec4(0.2f, 0.5f, 0.4f, 1.0f);
        pxl::Renderer::SetClearColour(clearColour);
        m_ClearColour = pxl::vec4(clearColour);

        m_VAO = std::make_shared<pxl::OpenGLVertexArray>();
        m_VBO = std::make_shared<pxl::OpenGLVertexBuffer>((uint32_t)(50000 * sizeof(pxl::Vertex)));
        m_IBO = std::make_shared<pxl::OpenGLIndexBuffer>(50000);
        m_Shader = std::make_shared<pxl::OpenGLShader>(vertexShaderCamera, fragmentShaderSource);

        pxl::BufferLayout layout;
        layout.Add(3, pxl::BufferDataType::Float, false); // vertex position
        layout.Add(2, pxl::BufferDataType::Float, false); // texture coords

        m_VAO->SetLayout(layout);

        m_VAO->SetVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(m_IBO);

        m_Camera = pxl::Camera::Create(pxl::CameraType::Perspective);
        m_Camera->SetPosition({-9.0f, -2.0f, -9.0f});
        m_Camera->SetRotation({20.0f, -135.3f, 0.0f});
        m_NextCameraFOV = m_Camera->GetFOV();

        // Load Assets
        pxl::ShaderLibrary::Add("camera.glsl", pxl::FileLoader::LoadShader("assets/shaders/camera.glsl"));

        auto texture1 = pxl::FileLoader::LoadTextureFromImage("assets/textures/stone.png");
        auto texture2 = pxl::FileLoader::LoadTextureFromImage("assets/textures/atlas.png");

        m_TextureLibrary.push_back(texture1);
        m_TextureLibrary.push_back(texture2);

        texture1->Bind();
        //m_Shader->SetUniformInt1("u_Texture", 0);

        //pxl::AudioManager::Add("wings", pxl::FileLoader::LoadAudioTrack("assets/audio/wings.mp3"));
        //pxl::AudioManager::Add("stone_dig", pxl::FileLoader::LoadAudioTrack("assets/audio/stone_dig.mp3"));

        m_AudioLibrary = pxl::AudioManager::GetLibrary();

        // Prepare cursor for camera control
        pxl::Input::SetCursorMode(pxl::CursorMode::Disabled);
        pxl::Input::SetRawInput(true);
        pxl::Input::SetCursorPosition(m_Window->GetWidth() / 2, m_Window->GetHeight() / 2);
        m_LastCursorPosition = pxl::Input::GetCursorPosition();

        #ifndef TA_RELEASE
            pxl::pxl_ImGui::Init(m_Window);
        #endif
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate(float dt)
    {
        m_CameraPosition = m_Camera->GetPosition();
        m_CameraRotation = m_Camera->GetRotation();
        auto cameraFOV = m_Camera->GetFOV();
        float cameraSpeed = 10.0f;
        glm::vec3 cameraForward = m_Camera->GetForwardVector();
        glm::vec3 cameraUp = m_Camera->GetUpVector();
        glm::vec3 cameraRight = m_Camera->GetRightVector();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            Application::Close();
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_TAB))
        {
            pxl::Input::SetCursorPosition(m_Window->GetWidth() / 2, m_Window->GetHeight() / 2);

            if (controllingCamera)
            {
                pxl::Input::SetCursorMode(pxl::CursorMode::Normal);
                //pxl::Input::SetCursorPosition(m_Window->GetWidth() / 2, m_Window->GetHeight() / 2);
                controllingCamera = false;
            }
            else
            {
                pxl::Input::SetCursorMode(pxl::CursorMode::Disabled);
                m_LastCursorPosition = pxl::Input::GetCursorPosition();
                m_MouseDelta = glm::vec2(0.0f);
                controllingCamera = true;
            }
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_P))
        {
            pxl::AudioManager::Play("wings");
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 10.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {
            m_CameraPosition += m_Camera->GetForwardVector() * cameraSpeed * dt;
        }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        // {
        //     m_CameraPosition += -cameraRight * cameraSpeed * dt;
        // }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
           m_CameraPosition += -m_Camera->GetForwardVector() * cameraSpeed * dt;
        }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        // {
        //     m_CameraPosition.x += cameraSpeed * dt;
        // }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
        {
            m_Window->NextWindowMode();
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            if (controllingCamera)
                m_NextCameraFOV -= 5.0f;
        }

        if (pxl::Input::IsMouseScrolledDown())
        {
            if (controllingCamera)
                m_NextCameraFOV += 5.0f;
        }

        float fovScrollValue = 100.0f;

        if (cameraFOV != m_NextCameraFOV)
        {
            if (cameraFOV < m_NextCameraFOV)
            {
                cameraFOV += fovScrollValue * dt;

                if (cameraFOV > m_NextCameraFOV)
                    cameraFOV = m_NextCameraFOV; // should be able to clamp these values
            }
            else if (cameraFOV > m_NextCameraFOV)
            {
                cameraFOV -= fovScrollValue * dt;

                if (cameraFOV < m_NextCameraFOV)
                    cameraFOV = m_NextCameraFOV;
            }
        }

        auto cursorPos = pxl::Input::GetCursorPosition();

        if (controllingCamera)
        {
            if (cursorPos != m_LastCursorPosition)
            {
                m_MouseDelta.x = cursorPos.x - m_LastCursorPosition.x;
                m_MouseDelta.y = cursorPos.y - m_LastCursorPosition.y;

                m_CameraRotation.x -= m_MouseDelta.y * m_SensitivityH;
                m_CameraRotation.y -= m_MouseDelta.x * m_SensitivityV;

                m_LastCursorPosition = cursorPos;
            }
        }

        m_CameraRotation.x = glm::clamp(m_CameraRotation.x, -90.0f, 90.0f);

        m_Camera->SetPosition({m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z});
        m_Camera->SetRotation({m_CameraRotation.x, m_CameraRotation.y, m_CameraRotation.z});
        m_Camera->SetFOV(cameraFOV);

        pxl::Renderer::ResetStats();
        pxl::Renderer::Clear();

        pxl::Renderer::Submit(m_VAO);
        pxl::Renderer::Submit(m_Shader);

        pxl::Renderer::StartBatch();

        //pxl::Renderer::DrawCube({ m_MeshPosition.x, m_MeshPosition.y, m_MeshPosition.z }, glm::vec3(1.0f), glm::vec3(1.0f), 0);

        for (uint32_t x = 0; x < 10; x++)
        {
            for (uint32_t y = 0; y < 10; y++)
            {
                for (uint32_t z = 0; z < 10; z++)
                {
                    //auto tex = (x + y + z) % 2;
                    pxl::Renderer::DrawCube({ x * 50.0f, y * 50.0f, z * 50.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
                }
            }
        }

        pxl::Renderer::EndBatch();

        pxl::Renderer::DrawIndexed(); // Draws any geometry that hasn't been flushed
    }

    void TestApplication::OnImGuiRender() // Function only gets called if ImGui is initialized
    {
            //ImGui::ShowDemoWindow();

            ImGui::SetNextWindowSize(ImVec2(300, 680), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(21, 21), ImGuiCond_FirstUseEver);
            
            ImGui::Begin("pxlFramework: Test App");

            ImGui::Text("FPS: %.2f (%.3fms)", pxl::Renderer::GetFPS(), pxl::Renderer::GetFrameTimeMS());
            ImGui::Text("Clear Colour:");
            ImGui::ColorEdit3("", &m_ClearColour.x);
            pxl::Renderer::SetClearColour(m_ClearColour);
            if (ImGui::Button("Reload Shader"))
            {
                m_Shader->Reload();
            }
            ImGui::Text("Camera FOV: %.2f", m_Camera->GetFOV());
            ImGui::Text("Camera Position: %.3f, %.3f, %.3f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
            ImGui::Text("Camera Rotation: %.3f, %.3f, %.3f", m_CameraRotation.x, m_CameraRotation.y, m_CameraRotation.z);

            auto stats = pxl::Renderer::GetStats();

            ImGui::Text("Renderer Stats:");
            ImGui::Text("  Draw Calls: %u", stats.DrawCalls);
            ImGui::Text("  Vertices: %u", stats.VertexCount);
            ImGui::Text("  Indices: %u", stats.IndiceCount);
            ImGui::Text("  Triangles: %u", stats.GetTriangleCount());

            auto cursorPos = pxl::Input::GetCursorPosition(); 

            ImGui::Text("Cursor Pos: %f, %f", cursorPos.x, cursorPos.y);
            ImGui::Text("Mouse Delta: %f, %f", m_MouseDelta.x, m_MouseDelta.y);

            static int selectedTextureInput = 0;
            static int lastSelectedTexture = 0;
            const char* textures[] = { "Stone", "Atlas" };

            ImGui::Combo("Texture", &selectedTextureInput, textures, IM_ARRAYSIZE(textures));

            if (selectedTextureInput != lastSelectedTexture)
            {
                switch(selectedTextureInput)
                {
                    case 0:
                        m_TextureLibrary.at(0)->Bind();
                        break;
                    case 1:
                        m_TextureLibrary.at(1)->Bind();
                        break;
                }

                lastSelectedTexture = selectedTextureInput;
            }

            ImGui::End();

            // Settings Window
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

            if (windowMode != m_Window->GetWindowMode())
            {
                switch (m_Window->GetWindowMode())
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

                windowMode = m_Window->GetWindowMode();
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
            static bool vsync = m_Window->GetGraphicsContext()->GetVSync();
            ImGui::Checkbox("VSync", &vsync);

            // Camera FOV
            static float cameraFOVInput = m_Camera->GetFOV(); // should retrieve cameras FOV
            static float cameraFOV = m_Camera->GetFOV();

            if (cameraFOV != m_NextCameraFOV)
            {
                cameraFOVInput = m_NextCameraFOV;
                cameraFOV = m_NextCameraFOV;
            }

            ImGui::SliderFloat("Camera FOV", &cameraFOVInput, 30.0f, 120.0f);

            // Apply Button
            if (ImGui::Button("Apply"))
            {
                m_Window->SetVSync(vsync);
                m_Window->SetMonitor(monitorIndex);
                m_Window->SetWindowMode(windowModeToSet);

                m_NextCameraFOV = cameraFOVInput;
            }

            ImGui::End();

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
            static std::string selectedAudioName;

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
    }
}