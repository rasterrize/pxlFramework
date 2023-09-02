#include "TestApplication.h"

#include <imgui.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        // float trianglepositions[6] = {
        //     -0.5f, -0.5f,
        //     0.5f, -0.5f,
        //     0.0f, 0.5f
        // };

        // float squarepositions[] = {
        //     -0.5f, -0.5f,
        //      0.5f, -0.5f,
        //      0.5f,  0.5f,
        //     -0.5f,  0.5f
        // };

        // uint32_t squareindices[] = {
        //     0, 1, 2,
        //     2, 3, 0
        // };

        // float cubepositions[] = {
        //     -0.5f, -0.5f,  0.5f,
        //      0.5f, -0.5f,  0.5f,
        //      0.5f,  0.5f,  0.5f,
        //     -0.5f,  0.5f,  0.5f,
        //     -0.5f, -0.5f, -0.5f,
        //      0.5f, -0.5f, -0.5f,
        //      0.5f,  0.5f, -0.5f,
        //     -0.5f,  0.5f, -0.5f
        // };

        uint32_t cubeIndices[] = { 
            0, 1, 2, 
            2, 3, 0, // front

            4, 5, 6,
            6, 7, 4, // back

            8, 9, 10,
            10, 11, 8, // left
    
            12, 13, 14,
            14, 15, 12, // top

            16, 17, 18,
            18, 19, 16, // right

            20, 21, 22,
            22, 23, 20 // bottom
        };

        std::string vertexShaderSource = R"(
            #version 460 core

            layout (location = 0) in vec3 a_Position;
            out vec3 v_Position;

            void main()
            {
                v_Position = a_Position;
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

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
        #else
            windowTitle = "pxlFramework Test App - Unknown Build Type";
        #endif
        
        m_Window = pxl::Window::Create({1280, 720, windowTitle, pxl::RendererAPIType::OpenGL});
        m_Window->SetPosition(1920 / 2 - 1280 / 2, 1080 / 2 - 720 / 2);
        m_Window->SetVSync(false);

        pxl::Renderer::Init(m_Window);

        pxl::Input::Init(m_Window);

        auto clearColour = pxl::vec4(20.0f / 255.0f, 24.0f / 255.0f, 28.0f / 255.0f, 1.0f); // pxl::vec4(0.2f, 0.5f, 0.4f, 1.0f);
        pxl::Renderer::SetClearColour(clearColour);
        m_ClearColour = pxl::vec4(clearColour);

        m_VAO = std::make_shared<pxl::OpenGLVertexArray>();
        m_VBO = std::make_shared<pxl::OpenGLVertexBuffer>(2000000);
        m_IBO = std::make_shared<pxl::OpenGLIndexBuffer>(2000000);
        m_Shader = std::make_shared<pxl::OpenGLShader>(vertexShaderCamera, fragmentShaderSource);

        pxl::ShaderLibrary::Add("camera.glsl", pxl::FileLoader::LoadShader("assets/shaders/camera.glsl"));

        pxl::Mesh mesh;

        // Front
        pxl::Vertex v1 = {{ -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }};
        pxl::Vertex v2 = {{ 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }};
        pxl::Vertex v3 = {{ 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f }};
        pxl::Vertex v4 = {{ -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f }};

        // Back
        pxl::Vertex v5 = {{ 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }};
        pxl::Vertex v6 = {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }};
        pxl::Vertex v7 = {{ -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }};
        pxl::Vertex v8 = {{ 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }};

        // For Textured Cube

        // Left
        pxl::Vertex v9  = {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }};
        pxl::Vertex v10 = {{ -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }};
        pxl::Vertex v11 = {{ -0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f }};
        pxl::Vertex v12 = {{ -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }};

        // Top
        pxl::Vertex v13 = {{ -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }};
        pxl::Vertex v14 = {{ 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f }};
        pxl::Vertex v15 = {{ 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }};
        pxl::Vertex v16 = {{ -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }};

        // Right
        pxl::Vertex v17 = {{ 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }};
        pxl::Vertex v18 = {{ 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }};
        pxl::Vertex v19 = {{ 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }};
        pxl::Vertex v20 = {{ 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f }};

        // Bottom
        pxl::Vertex v21 = {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }};
        pxl::Vertex v22 = {{ 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }};
        pxl::Vertex v23 = {{ 0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f }};
        pxl::Vertex v24 = {{ -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f }};

        mesh.Vertices.push_back(v1);
        mesh.Vertices.push_back(v2);
        mesh.Vertices.push_back(v3);
        mesh.Vertices.push_back(v4);
        mesh.Vertices.push_back(v5);
        mesh.Vertices.push_back(v6);
        mesh.Vertices.push_back(v7);
        mesh.Vertices.push_back(v8);
        mesh.Vertices.push_back(v9);
        mesh.Vertices.push_back(v10);
        mesh.Vertices.push_back(v11);
        mesh.Vertices.push_back(v12);
        mesh.Vertices.push_back(v13);
        mesh.Vertices.push_back(v14);
        mesh.Vertices.push_back(v15);
        mesh.Vertices.push_back(v16);
        mesh.Vertices.push_back(v17);
        mesh.Vertices.push_back(v18);
        mesh.Vertices.push_back(v19);
        mesh.Vertices.push_back(v20);
        mesh.Vertices.push_back(v21);
        mesh.Vertices.push_back(v22);
        mesh.Vertices.push_back(v23);
        mesh.Vertices.push_back(v24);

        for (int i = 0; i < 36; i++)
        {
            mesh.Indices.push_back(cubeIndices[i]);

        }

        m_CubeMesh = mesh;

        pxl::BufferLayout layout;
        layout.Add(3, pxl::BufferDataType::Float, false); // vertex position
        layout.Add(2, pxl::BufferDataType::Float, false); // texture coords

        m_VAO->SetLayout(layout);

        m_VAO->SetVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(m_IBO);

        m_Camera = pxl::Camera::Create(pxl::CameraType::Perspective);
        m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        m_NextCameraFOV = m_Camera->GetFOV();

        auto texture = pxl::FileLoader::LoadTextureFromImage("assets/textures/stone.png");
        texture->Bind();

        //m_Shader->SetUniformInt1("u_Texture", 0);

        #ifdef TA_DEBUG
        pxl::Input::SetCursorMode(pxl::CursorMode::Disabled);
        pxl::Input::SetRawInput(true);
        m_LastCursorPosition = pxl::Input::GetCursorPosition();
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
        float cameraSpeed = 1.0f;
        // float meshSpeed = 1.0f;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            Application::Close();
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_TAB))
        {
            if (controllingCamera)
            {
                pxl::Input::SetCursorMode(pxl::CursorMode::Normal);
                pxl::Input::SetCursorPosition(m_Window->GetWidth() / 2, m_Window->GetHeight() / 2);
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
            cameraSpeed *= 10.0f;
            //meshSpeed *= 10.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {
            m_CameraPosition.y += cameraSpeed * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        {
            m_CameraPosition.x -= cameraSpeed * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            m_CameraPosition.y -= cameraSpeed * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        {
            m_CameraPosition.x += cameraSpeed * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_Q))
        {
            m_CameraPosition.z -= cameraSpeed * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
        {
            m_CameraPosition.z += cameraSpeed * dt;
        }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_UP))
        // {
        //     m_MeshPosition.y += meshSpeed * dt;
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT))
        // {
        //     m_MeshPosition.x -= meshSpeed * dt;
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_DOWN))
        // {
        //     m_MeshPosition.y -= meshSpeed * dt;
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_RIGHT))
        // {
        //     m_MeshPosition.x += meshSpeed * dt;
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_DELETE))
        // {
        //     m_MeshPosition.z += meshSpeed * dt;
        // }
        // if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_PAGE_DOWN))
        // {
        //     m_MeshPosition.z -= meshSpeed * dt;
        // }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
        {
            m_Window->NextWindowMode();
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            m_NextCameraFOV -= 5.0f;
        }

        if (pxl::Input::IsMouseScrolledDown())
        {
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

        pxl::Renderer::Clear();

        pxl::Renderer::Submit(m_VAO);
        pxl::Renderer::Submit(m_Shader);
        //pxl::Renderer::Submit(m_CubeMesh);

        for (int x = 0; x < 10; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                for (int z = 0; z < 10; z++)
                {
                    m_CubeMesh.Translate(x * 3.0f, y * 3.0f, z * -3.0f);
                    pxl::Renderer::Submit(m_CubeMesh); // this submits the mesh vertices, indices, and transform
                }
            }
        }

        // for (int i = 0; i < 5; i++)
        // {
        //     m_CubeMesh.Translate(i * 2.0f, 0.0f, 0.0f);
        //     pxl::Renderer::Submit(m_CubeMesh); // this submits the mesh vertices, indices, and transform
        // }

        pxl::Renderer::BatchGeometry();

        pxl::Renderer::DrawIndexed();
    }

    void TestApplication::OnImGuiRender()
    {
        #ifdef TA_DEBUG
            // Function only gets called if ImGui is initialized
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
            ImGui::Text("Mesh Position: %.3f, %.3f, %.3f", m_MeshPosition.x, m_MeshPosition.y, m_MeshPosition.z);

            auto cursorPos = pxl::Input::GetCursorPosition(); 

            ImGui::Text("Cursor Pos: %f, %f", cursorPos.x, cursorPos.y);
            ImGui::Text("Mouse Delta: %f, %f", m_MouseDelta.x, m_MouseDelta.y);
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
        #endif
    }
}