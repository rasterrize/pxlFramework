#include "QuadsTest.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> QuadsTest::m_Window;
    std::shared_ptr<pxl::Camera> QuadsTest::m_Camera;

    glm::vec4 QuadsTest::m_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

    bool QuadsTest::m_ControllingCamera = true;

    glm::vec4 QuadsTest::m_QuadColour = { 0.180f, 0.293f, 0.819f, 1.0f };

    glm::vec2 QuadsTest::m_SelectedTile = { 0.0f, 0.0f };

    uint32_t QuadsTest::m_BlueQuadAmount = 10;
    uint32_t QuadsTest::m_OrangeQuadAmount = 10;

    std::shared_ptr<pxl::Texture> QuadsTest::m_StoneTexture;

    void QuadsTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'QuadsTest'";

        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);
        
        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Orthographic, 16.0f / 9.0f, -10.0f, 10.0f });
        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });
        m_Camera->SetZoom(2.5f);

        pxl::Renderer::SetClearColour(m_ClearColour);

        pxl::Renderer::SetQuadsCamera(m_Camera);

        m_StoneTexture = pxl::FileLoader::LoadTextureFromImage("assets/textures/stone.png");
    }

    void QuadsTest::OnUpdate(float dt)
    {
        auto cameraPosition = m_Camera->GetPosition();
        auto cameraRotation = m_Camera->GetRotation();
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
            cameraSpeed *= 3.0f;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
            cameraPosition.y += cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
            cameraPosition.x -= cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
            cameraPosition.y -= cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
            cameraPosition.x += cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_Q))
            cameraPosition.z += cameraSpeed * dt;
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
            cameraPosition.z -= cameraSpeed * dt;

        if (pxl::Input::IsMouseScrolledUp())
            cameraZoom -= cameraSpeed * 0.5f;

        if (pxl::Input::IsMouseScrolledDown())
            cameraZoom += cameraSpeed * 0.5f;

        m_Camera->SetPosition({ cameraPosition.x, cameraPosition.y, cameraPosition.z });
        m_Camera->SetZoom(cameraZoom);
    }

    void QuadsTest::OnRender()
    {
        pxl::Renderer::Clear();

        //pxl::Renderer::AddQuad({ -0.5f, -0.5f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(1.0f));

        for (uint32_t x = 0; x < m_BlueQuadAmount; x += 2)
        {
            for (uint32_t y = 0; y < m_BlueQuadAmount; y += 2)
            {
                pxl::Renderer::AddQuad({ x - (m_BlueQuadAmount / 2.0f), y - (m_BlueQuadAmount / 2.0f), 1.0f }, glm::vec3(0.0f), glm::vec2(1.0f), { 0.4f, 0.4f, 0.7f, 1.0f });
            }
        }

        //pxl::Renderer::AddQuad({ -0.5f, -0.5f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), { 1.0f, 0.5f, 0.3f, 1.0f });
        //pxl::Renderer::AddQuad({ 2.0f, 2.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(0.5f), { 1.0f, 0.5f, 0.3f, 1.0f });
        // pxl::Renderer::AddTexturedQuad({ 0.0f, 0.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), m_StoneTexture);
        // pxl::Renderer::AddTexturedQuad({ 2.0f, 2.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), m_StoneTexture);

        //pxl::Renderer::AddQuad(glm::vec3(0.0f));
    }

    void QuadsTest::OnGuiRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void QuadsTest::OnClose()
    {
    }
}