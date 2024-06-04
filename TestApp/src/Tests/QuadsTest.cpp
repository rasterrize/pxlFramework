#include "QuadsTest.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> QuadsTest::m_Window;
    std::shared_ptr<pxl::Camera> QuadsTest::m_Camera;
    std::shared_ptr<pxl::Shader> QuadsTest::m_Shader; 
    std::shared_ptr<pxl::GraphicsPipeline> QuadsTest::m_Pipeline;
    glm::vec4 QuadsTest::m_ClearColour;

    glm::vec3 QuadsTest::m_CameraPosition = glm::vec3(0.0f);
    glm::vec3 QuadsTest::m_CameraRotation = glm::vec3(0.0f);
    bool QuadsTest::m_ControllingCamera = true;

    glm::vec4 QuadsTest::m_QuadColour = { 0.180f, 0.293f, 0.819f, 1.0f };

    glm::vec2 QuadsTest::m_SelectedTile = { 0.0f, 0.0f };

    uint32_t QuadsTest::m_BlueQuadAmount = 100;
    uint32_t QuadsTest::m_OrangeQuadAmount = 100;

    std::shared_ptr<pxl::Texture2D> QuadsTest::m_StoneTexture;

    void QuadsTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'QuadsTest'";

        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_Window);

        pxl::Input::Init(m_Window);
        
        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Orthographic, 16.0f / 9.0f, -10.0f, 10.0f });
        m_Camera->SetPosition({ 0.0f, 0.0f, 0.0f });
        m_Camera->SetZoom(2.5f);

        m_ClearColour = glm::vec4(0.078f, 0.094f, 0.109f, 1.0f);
        pxl::Renderer::SetClearColour(m_ClearColour);

        pxl::Renderer::SetQuadsCamera(m_Camera);

        m_StoneTexture = pxl::FileLoader::LoadTextureFromImage("assets/textures/stone.png");
    }

    void QuadsTest::OnUpdate(float dt)
    {
        m_CameraPosition = m_Camera->GetPosition();
        m_CameraRotation = m_Camera->GetRotation();
        auto cameraSpeed = 2.0f;
        auto cameraZoom = m_Camera->GetZoom();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            m_Window->ToggleVSync();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 3.0f;
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
            m_CameraPosition.z += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
        {
            m_CameraPosition.z -= cameraSpeed * dt;
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraZoom -= cameraSpeed * 0.5f;
        }

        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraZoom += cameraSpeed * 0.5f;
        }

        m_Camera->SetPosition({ m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z });
        m_Camera->SetZoom(cameraZoom);
    }

    void QuadsTest::OnRender()
    {
        pxl::Renderer::Clear();

        //pxl::Renderer::AddQuad({ -0.5f, -0.5f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(1.0f));

        // for (int x = 0; x < m_BlueQuadAmount; x += 2)
        // {
        //     for (int y = 0; y < m_BlueQuadAmount; y += 2)
        //     {
        //         pxl::Renderer::AddQuad({ x - (m_BlueQuadAmount / 2.0f), y - (m_BlueQuadAmount / 2.0f), 1.0f }, glm::vec3(0.0f), glm::vec2(1.0f), { 0.4f, 0.4f, 0.7f, 1.0f });
        //     }
        // }

        pxl::Renderer::AddQuad({ -0.5f, -0.5f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), { 1.0f, 0.5f, 0.3f, 1.0f });
        //pxl::Renderer::AddQuad({ 2.0f, 2.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(0.5f), { 1.0f, 0.5f, 0.3f, 1.0f });
        // pxl::Renderer::AddTexturedQuad({ 0.0f, 0.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), m_StoneTexture);
        // pxl::Renderer::AddTexturedQuad({ 2.0f, 2.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), m_StoneTexture);

        //pxl::Renderer::AddQuad(glm::vec3(0.0f));
    }

    void QuadsTest::OnImGuiRender()
    {

    }

    void QuadsTest::OnClose()
    {
    }
}