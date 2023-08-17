#include "TestApplication.h"

#include <imgui.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        float trianglepositions[6] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.0f, 0.5f
        };

        float squarepositions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
        };

        unsigned int squareindices[] = {
            0, 1, 2,
            2, 3, 0
        };

        float cubepositions[] = {
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f
        };

        unsigned int cubeindices[] = { 
            0, 1, 2, 
            2, 3, 0, // front

            1, 5, 6,
            6, 2, 1, // right

            5, 4, 7,
            7, 6, 5, // back

            4, 0, 3,
            3, 7, 4, // left

            3, 2, 6,
            6, 7, 3, // top

            0, 1, 5,
            5, 4, 0 // bottom
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

            uniform mat4 u_Model;
            uniform mat4 u_Projection;
            uniform mat4 u_View;

            void main()
            {
                v_Position = a_Position;
                v_TexCoords = a_TexCoords;
                gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 450 core
            
            layout (location = 0) out vec4 color;

            in vec3 v_Position;
            in vec2 v_TexCoords;

            uniform sampler2D u_Texture;

            void main()
            {
                //color = vec4(1.0, 0.2, 0.4, 1.0);
                //color = vec4(v_Position, 1.0);
                color = texture(u_Texture, v_TexCoords);
                // * vec4(v_Position.x + 0.6, v_Position.y + 0.6, v_Position.z, 1.0);
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

        pxl::Renderer::Init(m_Window);

        pxl::Input::Init(m_Window);

        auto clearColour = pxl::vec4(20.0f / 255.0f, 24.0f / 255.0f, 28.0f / 255.0f, 1.0f); // pxl::vec4(0.2f, 0.5f, 0.4f, 1.0f);
        pxl::Renderer::SetClearColour(clearColour);
        m_ClearColour = pxl::vec4(clearColour);

        m_VAO = std::make_shared<pxl::OpenGLVertexArray>();
        m_VBO = std::make_shared<pxl::OpenGLVertexBuffer>();
        m_IBO = std::make_shared<pxl::OpenGLIndexBuffer>(36, cubeindices);
        m_Shader = std::make_shared<pxl::OpenGLShader>(vertexShaderCamera, fragmentShaderSource);

        pxl::ShaderLibrary::Add("camera.glsl", pxl::FileLoader::LoadShader("assets/shaders/camera.glsl"));

        std::shared_ptr<pxl::Mesh> mesh = std::make_shared<pxl::Mesh>();

        pxl::Vertex v0 = { -0.5f, -0.5f, 0.5f, 0.0f, 0.0f };
        pxl::Vertex v1 = { 0.5f, -0.5f, 0.5f, 1.0f, 0.0f };
        pxl::Vertex v2 = { 0.5f, 0.5f, 0.5f, 1.0f, 1.0f };
        pxl::Vertex v3 = { -0.5f, 0.5f, 0.5f, 0.0f, 1.0f };
        pxl::Vertex v4 = { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f };
        pxl::Vertex v5 = { 0.5f, -0.5f, -0.5f, 1.0f, 0.0f };
        pxl::Vertex v6 = { 0.5f, 0.5f, -0.5f, 1.0f, 1.0f };
        pxl::Vertex v7 = { -0.5f, 0.5f, -0.5f, 0.0f, 1.0f };

        mesh->Vertices.push_back(v0);
        mesh->Vertices.push_back(v1);
        mesh->Vertices.push_back(v2);
        mesh->Vertices.push_back(v3);
        mesh->Vertices.push_back(v4);
        mesh->Vertices.push_back(v5);
        mesh->Vertices.push_back(v6);
        mesh->Vertices.push_back(v7);

        for (int i = 0; i < 36; i++)
        {
            mesh->Indices.push_back(cubeindices[i]);
        }

        m_CubeMeshes.push_back(mesh);

        pxl::BufferLayout layout;
        layout.Add(3, pxl::BufferDataType::Float, false); // vertex position
        layout.Add(2, pxl::BufferDataType::Float, false); // texture coords

        m_VAO->SetLayout(layout);

        m_VAO->SetVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(m_IBO);

        m_Camera = pxl::Camera::Create(pxl::CameraType::Perspective);
        m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

        auto texture = pxl::FileLoader::LoadTextureFromImage("assets/textures/stone.png");
        texture->Bind();

        //m_Shader->SetUniformInt1("u_Texture", 0);

        #ifdef TA_DEBUG
            pxl::pxl_ImGui::Init(m_Window);
        #endif
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate(float dt)
    {
        m_CameraPosition = m_Camera->GetPosition();
        float cameraSpeed = 1.0f;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            Application::Close();
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 2.0f;
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
            m_CameraPosition.z -= 2.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
        {
            m_CameraPosition.z += 2.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_UP))
        {
            m_MeshPosition.y += 1.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT))
        {
            m_MeshPosition.x -= 1.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_DOWN))
        {
            m_MeshPosition.y -= 1.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_RIGHT))
        {
            m_MeshPosition.x += 1.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_DELETE))
        {
            m_MeshPosition.z += 1.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_PAGE_DOWN))
        {
            m_MeshPosition.z -= 1.0f * dt;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
        {
            m_Window->NextWindowMode();
        }

        m_Camera->SetPosition(glm::vec3(m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z));
        m_CubeMeshes[0]->Translate(m_MeshPosition.x, m_MeshPosition.y, m_MeshPosition.z);

        pxl::Renderer::Clear();

        pxl::Renderer::Submit(m_VAO);
        pxl::Renderer::Submit(m_Shader);
        pxl::Renderer::Submit(m_CubeMeshes[0]); // this submits the mesh vertices, indices, and transform

        
        //float offsetX = 0.0f, offsetY = 0.0f, offsetZ = 0.0f;

        //for (int i = 0; i < m_Meshes.size(); i++)
        //{
            //for (int n = 0; n <= 5; n++)
            //{
                //pxl::Renderer::Submit(m_CubeMeshes[0], m_MeshPosition.x, m_MeshPosition.y, m_MeshPosition.z);
                //offsetX += 1;
            //}
        //}

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
            ImGui::Text("MeshPosition.x: %.2f", m_MeshPosition.x);
            ImGui::Text("MeshPosition.y: %.2f", m_MeshPosition.y);
            ImGui::Text("MeshPosition.z: %.2f", m_MeshPosition.z);
            ImGui::End();

            // Settings Window
            ImGui::SetNextWindowSize(ImVec2(330, 200), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(929, 21), ImGuiCond_FirstUseEver);
            ImGui::Begin("Settings");

            // Monitor select
            static uint8_t monitor = 1;
            int step = 1;
            ImGui::InputScalar("Monitor", ImGuiDataType_U8, &monitor, &step, NULL, NULL, NULL);

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
            static float cameraFOV = m_Camera->GetFOV(); // should retrieve cameras FOV
            ImGui::SliderFloat("Camera FOV", &cameraFOV, 30.0f, 120.0f);

            // Apply Button
            if (ImGui::Button("Apply"))
            {
                m_Window->SetVSync(vsync);
                m_Window->SetMonitor(monitor);
                m_Window->SetWindowMode(windowModeToSet);

                m_Camera->SetFOV(cameraFOV);
            }

            ImGui::End();
        #endif
    }
}