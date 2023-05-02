#include "TestApplication.h"

namespace TestApp
{
    TestApplication::TestApplication()
    {

        float positions[6] = {
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
            #version 450 core

            layout (location = 0) in vec3 a_Position;
            out vec3 v_Position;

            void main()
            {
                v_Position = a_Position;
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

        std::string vertexShaderCamera = R"(
            #version 450 core

            layout (location = 0) in vec3 a_Position;
            out vec3 v_Position;

            uniform mat4 u_Model;
            uniform mat4 u_Projection;
            uniform mat4 u_View;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 450 core
            
            layout (location = 0) out vec4 color;
            in vec3 v_Position;

            void main()
            {
                //color = vec4(1.0, 0.2, 0.4, 1.0);
                color = vec4(v_Position, 1.0);
            }
        )";

        // pxl::Mesh mesh;
        // auto vertexcount = (sizeof(cubepositions) / sizeof(cubepositions[0]));
        // for (int i = 0; i < vertexcount; i += 3)
        // {
        //     glm::vec3 vertex;
        //     vertex.x = cubepositions[i];
        //     vertex.y = cubepositions[i + 1];
        //     vertex.z = cubepositions[i + 2];

        //     mesh.m_Vertices.push_back(vertex);
        // }

        // auto indicecount = (sizeof(cubeindices) / sizeof(cubeindices[0]));
        // for (int i = 0; i < indicecount; i++)
        // {
        //     mesh.m_Indices.push_back(cubeindices[i]);
        // }

        pxl::Window::Init(1280, 720, "pxlFramework", pxl::RendererAPIType::OpenGL);
        pxl::Input::Init();

        pxl::Renderer::SetClearColour(0.2, 0.5, 0.4, 1.0);

        pxl::OpenGLVertexArray* vertexArray = new pxl::OpenGLVertexArray();
        pxl::OpenGLVertexBuffer* vertexBuffer = new pxl::OpenGLVertexBuffer(sizeof(cubepositions), cubepositions);
        pxl::OpenGLIndexBuffer* indexBuffer = new pxl::OpenGLIndexBuffer(36, cubeindices);
        pxl::OpenGLShader* shader = new pxl::OpenGLShader(vertexShaderCamera, fragmentShaderSource);

        pxl::BufferLayout layout;
        layout.Add(3, pxl::BufferDataType::Float, false);
        vertexArray->SetLayout(layout);

        pxl::Renderer::Submit(vertexArray);
        pxl::Renderer::Submit(vertexBuffer);
        pxl::Renderer::Submit(indexBuffer);
        pxl::Renderer::Submit(shader);

        pxl::Camera::Init(pxl::CameraType::Perspective);
        pxl::Camera::SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

        pxl::Texture* texture = pxl::TextureLoader::Load("Textures/stone.png");

        //pxl::pxl_ImGui::Init();
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate()
    {
        auto cameraPosition = pxl::Camera::GetPosition();
        //pxl::Logger::LogWarn(std::to_string(cameraPosition.x) + ", " + std::to_string(cameraPosition.y) + ", " + std::to_string(cameraPosition.z));
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F11))
        {
            pxl::Window::NextWindowMode();
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {
            cameraPosition.y += 0.01f;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        {
            cameraPosition.x -= 0.01f;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            cameraPosition.y -= 0.01f;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        {
            cameraPosition.x += 0.01f;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_Q))
        {
            cameraPosition.z -= 0.01f;
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
        {
            cameraPosition.z += 0.01f;
        }

        pxl::Camera::SetPosition(glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));

        pxl::Renderer::Clear();

        pxl::Renderer::DrawIndexed();
    }
}