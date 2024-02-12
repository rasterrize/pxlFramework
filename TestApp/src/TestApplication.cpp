#include "TestApplication.h"

#include <imgui.h>
#include <thread>
#include <chrono>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        std::string vertexShader = R"(
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

        std::string fragmentShader = R"(
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

        // std::string modelVertexShader = R"(
        //     #version 460 core

        //     layout (location = 0) in vec3 a_Position;
        //     layout (location = 1) in vec4 a_Colour;
        //     //layout (location = 2) in vec2 a_TexCoords;

        //     out vec3 v_Position;
        //     out vec4 v_Colour;
        //     out vec2 v_TexCoords;

        //     uniform mat4 u_VP;

        //     void main()
        //     {
        //         v_Position = a_Position;
        //         v_Colour = a_Colour;
        //         v_TexCoords = a_TexCoords;
        //         gl_Position = u_VP * vec4(a_Position, 1.0);
        //     }
        // )";

        // std::string modelFragmentShader = R"(
        //     #version 460 core
            
        //     layout (location = 0) out vec4 color;

        //     in vec3 v_Position;
        //     in vec4 v_Colour;
        //     in vec2 v_TexCoords;

        //     uniform sampler2D u_Texture;

        //     void main()
        //     {
        //         color = v_Colour;
        //         //color = texture(u_Texture, v_TexCoords) * v_Colour;
        //     }
        // )";

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
        
        m_Window = pxl::Window::Create({ 1280, 720, windowTitle, windowRendererAPI });
        m_Window->SetPosition(1920 / 2 - 1280 / 2, 1080 / 2 - 720 / 2); // should be part of the window creation
        //m_Window->SetVSync(true);
        m_Window->SetWindowMode(windowMode);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        APP_LOG_INFO("Finished creating application");

        //m_ClearColour = glm::vec4(0.078f, 0.094f, 0.109f, 1.0f);
        //pxl::Renderer::SetClearColour(m_ClearColour);

        //m_Shader = std::make_shared<pxl::OpenGLShader>(vertexShader, fragmentShader);

        m_Camera = pxl::Camera::Create(pxl::CameraType::Orthographic, { 16.0f / 9.0f, -10.0, 10.0f });
        m_Camera->SetPosition({0.0f, 0.0f, 0.0f});

        auto vertBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/vert.spv");
        auto fragBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/frag.spv");

        m_VulkanTestShader = pxl::Shader::Create(vertBin, fragBin);

        pxl::BufferLayout layout;
        layout.Add(3, pxl::BufferDataType::Float, false); // vertex position
        layout.Add(4, pxl::BufferDataType::Float, false); // colour
        layout.Add(2, pxl::BufferDataType::Float, false); // texture coords

        m_VulkanTestPipeline = pxl::GraphicsPipeline::Create(m_VulkanTestShader, layout);

        // m_Camera->SetRotation({0.0f, 0.0f, 0.0f});

        // #ifndef TA_RELEASE
        //     pxl::pxl_ImGui::Init(m_Window);
        // #endif

        //pxl::Renderer::AddStaticQuad();
        //pxl::Renderer::StaticQuadsReady();
    }

    TestApplication::~TestApplication()
    {
        // Save framework settings // TODO: this should be semi-automatic and handled by the application class
        auto frameworkSettings = pxl::FrameworkConfig::GetSettings();

        //frameworkSettings.WindowMode = m_Window->GetWindowMode();
        //frameworkSettings.RendererAPI // TODO

        pxl::FrameworkConfig::SetSettings(frameworkSettings);
    }

    void TestApplication::OnUpdate(float dt)
    {
        // m_CameraPosition = m_Camera->GetPosition();
        // m_CameraRotation = m_Camera->GetRotation();
        // auto cameraSpeed = 2.0f;
        // auto cameraZoom = m_Camera->GetZoom();
        // auto cursorPos = pxl::Input::GetCursorPosition();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            m_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            m_Window->ToggleVSync();

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_TAB))
        // {
        //     if (controllingCamera)
        //         controllingCamera = false;
        //     else
        //         controllingCamera = true;
        // }

        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        // {
        //     cameraSpeed *= 5.0f;
        // }

        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        // {
        //     if (controllingCamera)
        //     {
        //         m_CameraPosition.y += cameraSpeed * dt;
        //     }
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        // {
        //     if (controllingCamera)
        //     {
        //         m_CameraPosition.x -= cameraSpeed * dt;
        //     }
        //     else
        //     {
        //         m_PlayerPosition.x -= 1.0f * dt;
        //     }
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        // {
        //     if (controllingCamera)
        //     {
        //         m_CameraPosition.y -= cameraSpeed * dt;
        //     }
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        // {
        //     if (controllingCamera)
        //     {
        //         m_CameraPosition.x += cameraSpeed * dt;
        //     }
        //     else
        //     {
        //         m_PlayerPosition.x += 1.0f * dt;
        //     }
        // }

        // if (pxl::Input::IsMouseScrolledUp())
        // {
        //     cameraZoom -= cameraSpeed * 0.5f;
        // }
        // if (pxl::Input::IsMouseScrolledDown())
        // {
        //     cameraZoom += cameraSpeed * 0.5f;
        // }

        // m_Camera->SetPosition({m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z});
        // m_Camera->SetZoom(cameraZoom);

        // // Ray casting
        // // convert to normalized device coords (-1 to 1 range on all 3 axis)
        // auto windowWidth = m_Window->GetWidth();
        // auto windowHeight = m_Window->GetHeight();
        // float x = (2.0f * cursorPos.x) / windowWidth - 1.0f;
        // float y = 1.0f - (2.0f * cursorPos.y) / windowHeight;
        // float z = 1.0f;
    }

    void TestApplication::OnRender()
    {
        pxl::Renderer::Begin();

        pxl::Renderer::Clear();

        //pxl::Renderer::Submit(m_Shader, m_Camera);
        pxl::Renderer::Submit(m_VulkanTestPipeline);
        
        //pxl::Renderer::AddQuad({ -0.5f, -0.5f, 0.0f });
        //pxl::Renderer::DrawStaticQuads();

        pxl::Renderer::End();

    }

    void TestApplication::OnImGuiRender() // Function only gets called if ImGui is initialized
    {
            //ImGui::ShowDemoWindow();

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
            ImGui::Text("Camera Position: %.3f, %.3f, %.3f", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
            ImGui::Text("Camera Rotation: %.3f, %.3f, %.3f", m_CameraRotation.x, m_CameraRotation.y, m_CameraRotation.z);

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

            ImGui::SliderFloat("Camera FOV", &cameraFOVInput, 30.0f, 120.0f);

            // Apply Button
            if (ImGui::Button("Apply"))
            {
                m_Window->SetVSync(vsync);
                m_Window->SetMonitor(monitorIndex);
                m_Window->SetWindowMode(windowModeToSet);

                cameraFOV = cameraFOVInput;
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