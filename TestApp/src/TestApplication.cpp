#include "TestApplication.h"

#include <imgui.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        std::string vertexShaderCamera = R"(
            #version 460 core

            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec4 a_Colour;
            layout (location = 2) in vec2 a_TexCoords;

            out vec3 v_Position;
            out vec4 v_Colour;
            out vec2 v_TexCoords;

            uniform mat4 u_VP;

            void main()
            {
                v_Position = a_Position;
                v_Colour = a_Colour;
                v_TexCoords = a_TexCoords;
                gl_Position = u_VP * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 460 core
            
            layout (location = 0) out vec4 color;

            in vec3 v_Position;
            in vec4 v_Colour;
            in vec2 v_TexCoords;

            uniform sampler2D u_Texture;

            void main()
            {
                color = v_Colour;
                //color = texture(u_Texture, v_TexCoords) * v_Colour;
            }
        )";

        std::string windowTitle = "pxlFramework Test App";
        std::string buildType = "Unknown Build Type";
        std::string rendererAPIType = "Unknown Renderer API";
        pxl::RendererAPIType windowRendererAPI = pxl::RendererAPIType::OpenGL;

        #ifdef TA_DEBUG
            buildType = "Debug x64";
        #elif TA_RELEASE
            buildType = "Release x64";
        #elif TA_DIST
            buildType = "Distribute x64";
        #endif

        switch (windowRendererAPI)
        {
            case pxl::RendererAPIType::None:
                rendererAPIType = "No Renderer API";
                break;
            case pxl::RendererAPIType::OpenGL:
                rendererAPIType = "OpenGL";
                break;
            case pxl::RendererAPIType::Vulkan:
                rendererAPIType = "Vulkan";
                break;
        }

        windowTitle = "pxlFramework Test App - " + buildType + " - " + rendererAPIType;
        
        m_Window = pxl::Window::Create({1280, 720, windowTitle, windowRendererAPI});
        m_Window->SetPosition(1920 / 2 - 1280 / 2, 1080 / 2 - 720 / 2);
        m_Window->SetVSync(true);

        pxl::Renderer::Init(m_Window);

        pxl::Input::Init(m_Window);

        pxl::AudioManager::Init(m_Window);

        auto clearColour = pxl::vec4(0.078f, 0.094f, 0.109f, 1.0f); // pxl::vec4(0.2f, 0.5f, 0.4f, 1.0f);
        pxl::Renderer::SetClearColour(clearColour);
        m_ClearColour = pxl::vec4(clearColour);

        m_Shader = std::make_shared<pxl::OpenGLShader>(vertexShaderCamera, fragmentShaderSource);

        m_Camera = pxl::Camera::Create(pxl::CameraType::Orthographic);
        m_Camera->SetPosition({0.0f, 0.0f, 0.0f});
        m_Camera->SetRotation({0.0f, 0.0f, 0.0f});
        m_NextCameraFOV = m_Camera->GetFOV();

        // Load Assets
        pxl::ShaderLibrary::Add("camera.glsl", pxl::FileLoader::LoadShader("assets/shaders/camera.glsl"));

        auto stoneTexture = pxl::FileLoader::LoadTextureFromImage("assets/textures/stone.png");
        auto atlasTexture = pxl::FileLoader::LoadTextureFromImage("assets/textures/atlas.png");
        auto cursorTexture = pxl::FileLoader::LoadTextureFromImage("assets/textures/cursor@2x.png");

        m_TextureLibrary.push_back(stoneTexture);
        m_TextureLibrary.push_back(atlasTexture);
        m_TextureLibrary.push_back(cursorTexture);

        cursorTexture->Bind();
        m_Shader->SetUniformInt1("u_Texture", 0);

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
        float cameraSpeed = 2.0f;
        auto cameraZoom = m_Camera->GetZoom();
        auto cursorPos = pxl::Input::GetCursorPosition();


        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            Application::Close();
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
        {
            m_Window->NextWindowMode();
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
        {
            m_Window->ToggleVSync();
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_TAB))
        {
            pxl::Input::SetCursorPosition(m_Window->GetWidth() / 2, m_Window->GetHeight() / 2);

            if (controllingCamera)
            {
                pxl::Input::SetCursorMode(pxl::CursorMode::Normal);
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
            cameraZoom -= 1.0f;
        }
        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraZoom += 1.0f;
        }

        //std::clamp(cameraZoom, 1.0f, 100.0f);

        m_Camera->SetPosition({m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z});
        m_Camera->SetZoom(cameraZoom);
    }

    void TestApplication::OnRender()
    {
        pxl::Renderer::ResetStats();
        pxl::Renderer::Clear();

        pxl::Renderer::Submit(m_Shader, m_Camera);

        pxl::Renderer::Begin();

        // Draw Grid
        for (int v = 0; v < 101; v++)
        {
            pxl::Renderer::AddLine({v - 50.0f, -50.0f, -0.1f }, {v - 50.0f, 50.0f, -1.0f }, glm::vec3(0.0f), glm::vec3(1.0f), { 0.2f, 0.2f, 0.2f, 1.0f });
        }

        for (int h = 0; h < 101; h++)
        {
            pxl::Renderer::AddLine({ -50.0f, h - 50.0f, -0.1f }, {50.0f, h - 50.0f, -1.0f }, glm::vec3(0.0f), glm::vec3(1.0f), { 0.2f, 0.2f, 0.2f, 1.0f });
        }

        // Draw Level
        pxl::Renderer::AddQuad({ 0.0f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 1.0f, 1.0f }, m_QuadColour);
        pxl::Renderer::AddQuad(m_PlayerPosition, glm::vec3(0.0f), glm::vec3(0.5f), glm::vec4(1.0f));

        pxl::Renderer::End();

        pxl::Renderer::Draw();
    }

    void TestApplication::OnImGuiRender() // Function only gets called if ImGui is initialized
    {
            //ImGui::ShowDemoWindow();

            ImGui::SetNextWindowSize(ImVec2(300, 680), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(21, 21), ImGuiCond_FirstUseEver);
            
            ImGui::Begin("pxlFramework: Test App");

            ImGui::Text("FPS: %.2f (%.3fms)", pxl::Renderer::GetFPS(), pxl::Renderer::GetFrameTimeMS());
            ImGui::Text("Clear Colour:");
            ImGui::ColorEdit3("Clear Colour", &m_ClearColour.x);
            ImGui::Text("Quad Colour:");
            ImGui::ColorEdit3("Quad Colour", &m_QuadColour.x);
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
            ImGui::Text("  Indices: %u", stats.IndexCount);
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