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

    int QuadsTest::m_BlueQuadAmount = 100;
    int QuadsTest::m_OrangeQuadAmount = 100;

    void QuadsTest::OnStart(const std::shared_ptr<pxl::Window>& window)
    {
        m_Camera = pxl::Camera::Create(pxl::CameraType::Orthographic, { 16.0f / 9.0f, -10.0, 10.0f });
        m_Camera->SetPosition({ 0.0f, 0.0f, 0.0f });

        m_ClearColour = glm::vec4(0.078f, 0.094f, 0.109f, 1.0f);
        pxl::Renderer::SetClearColour(m_ClearColour);
    }

    void QuadsTest::OnUpdate(float dt)
    {
        m_CameraPosition = m_Camera->GetPosition();
        m_CameraRotation = m_Camera->GetRotation();
        auto cameraSpeed = 2.0f;
        auto cameraZoom = m_Camera->GetZoom();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
        {
            cameraSpeed *= 5.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
        {

        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
        {
            
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
        {
            
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
    }

    void QuadsTest::OnClose()
    {
    }
}