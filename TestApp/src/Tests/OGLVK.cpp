#include "OGLVK.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> OGLVK::m_Window;
    std::shared_ptr<pxl::Shader> OGLVK::m_Shader; 
    std::shared_ptr<pxl::GraphicsPipeline> OGLVK::m_Pipeline;
    std::shared_ptr<pxl::Camera> OGLVK::m_Camera;

    void OGLVK::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'OGLVK'";
        
        m_Window = pxl::Window::Create(windowSpecs);
        
        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        pxl::Renderer::SetClearColour({ 0.078f, 0.094f, 0.109f, 1.0f });

        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Perspective, 16.0f / 9.0f, 1.0f, 1000.0f });
        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetQuadsCamera(m_Camera);
    }

    void OGLVK::OnUpdate(float dt)
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

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_1))
            m_Window->SetMonitor(pxl::Window::GetPrimaryMonitor());

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_2))
            m_Window->SetMonitor(pxl::Window::GetMonitors()[1]);

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
        
    void OGLVK::OnRender()
    {
        PXL_PROFILE_SCOPE;
        
        pxl::Renderer::Clear();

        constexpr uint32_t quadCount = 100;

        for (uint32_t x = 0; x < quadCount; x += 2)
        {
            for (uint32_t y = 0; y < quadCount; y += 2)
            {
                pxl::Renderer::AddQuad({ x - (quadCount / 2.0f) - 0.2f, y - (quadCount / 2.0f) - 0.2f, 0.0f }, glm::vec3(0.0f), glm::vec2(1.4f), glm::vec4(1.0f));
                pxl::Renderer::AddQuad({ x - (quadCount / 2.0f) - 0.1f, y - (quadCount / 2.0f) - 0.1f, 0.1f }, glm::vec3(0.0f), glm::vec2(1.2f), { 0.8f, 0.5f, 0.3f, 1.0f });
                pxl::Renderer::AddQuad({ x - (quadCount / 2.0f), y - (quadCount / 2.0f), 0.2f }, glm::vec3(0.0f), glm::vec2(1.0f), { 0.4f, 0.4f, 0.7f, 1.0f });
            }
        }
    }

    void OGLVK::OnImGuiRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void OGLVK::OnClose()
    {
    }
}