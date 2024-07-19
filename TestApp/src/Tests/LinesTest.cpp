#include "LinesTest.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> LinesTest::m_Window = nullptr;
    std::shared_ptr<pxl::Camera> LinesTest::m_Camera = nullptr;

    static constexpr int64_t RPCCLIENT_ID = 1141683223064231946;
    
    void LinesTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'LinesTest'";
        
        m_Window = pxl::Window::Create(windowSpecs);
        
        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        pxl::Renderer::SetClearColour({ 0.078f, 0.094f, 0.109f, 1.0f });

        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Perspective, 16.0f / 9.0f, 1.0f, 1000.0f });
        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetQuadsCamera(m_Camera);

        std::string rendererAPIString = pxl::EnumStringHelper::RendererAPITypeToString(windowSpecs.RendererAPI);

        pxl::DiscordRPC::Init(RPCCLIENT_ID);
        pxl::DiscordRPC::SetPresence({ RPCCLIENT_ID, "Test App", "Running test 'LinesTest' utilizing " + rendererAPIString, pxl::DiscordRPCActivityType::Playing, "ta"});
    }

    void LinesTest::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;
        
        auto cameraPosition = m_Camera->GetPosition();
        auto cameraZoom = m_Camera->GetZoom();
        auto cameraFOV = m_Camera->GetFOV();
        auto cameraSpeed = 2.0f;
        
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            m_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            m_Window->ToggleVSync();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 3.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_CONTROL))
        {
            cameraSpeed *= 0.5f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {
            cameraPosition.y += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        {
            cameraPosition.x -= cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            cameraPosition.y -= cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        {
            cameraPosition.x += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_Q))
        {
            cameraPosition.z += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
        {
            cameraPosition.z -= cameraSpeed * dt;
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraZoom -= cameraSpeed * 0.5f;
            cameraFOV -= cameraSpeed * 0.5f;
        }

        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraZoom += cameraSpeed * 0.5f;
            cameraFOV += cameraSpeed * 0.5f;
        }

        m_Camera->SetPosition(cameraPosition);
        m_Camera->SetZoom(cameraZoom);
        m_Camera->SetFOV(cameraFOV);
    }

    void LinesTest::OnRender()
    {
        PXL_PROFILE_SCOPE;
        
        pxl::Renderer::Clear();

        pxl::Renderer::AddLine({ -0.5f, 1.0f, 1.0f }, { 0.5f, -1.0f, -1.0f }, glm::vec3(0.0f), glm::vec3(1.0f), { 0.4f, 0.8f, 0.2f, 1.0f });
    }

    void LinesTest::OnImGuiRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void LinesTest::OnClose()
    {
    }
}