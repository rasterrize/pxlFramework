#include "ModelViewer.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> ModelViewer::m_Window;
    std::shared_ptr<pxl::Camera> ModelViewer::m_Camera;
    glm::vec4 ModelViewer::m_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

    static std::shared_ptr<pxl::Mesh> s_CurrentMesh;
    static std::shared_ptr<pxl::Mesh> s_AltMesh;
    static glm::vec3 s_MeshRotation = { 45.0f, 45.0f, 0.0f };

    void ModelViewer::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'ModelViewer'";

        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        pxl::Renderer::SetClearColour(m_ClearColour);

        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Perspective, 16.0f / 9.0f, 0.01f, 1000.0f });
        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetQuadsCamera(m_Camera);

        s_CurrentMesh = pxl::FileSystem::LoadOBJ("assets/models/golf_ball.obj");
    }

    void ModelViewer::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;
        
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            m_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            m_Window->ToggleVSync();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_M))
            std::swap(s_CurrentMesh, s_AltMesh);

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            auto cameraPos = m_Camera->GetPosition();
            m_Camera->SetPosition({ cameraPos.x, cameraPos.y, cameraPos.z + 0.5f * dt });
        }

        s_MeshRotation.x += 10.0f * dt;
        s_MeshRotation.y += 10.0f * dt;
    }
        
    void ModelViewer::OnRender()
    {
        PXL_PROFILE_SCOPE;
        
        pxl::Renderer::Clear();

        pxl::Renderer::DrawMesh(s_CurrentMesh, glm::vec3(0.0f), s_MeshRotation, glm::vec3(1.0f));
    }

    void ModelViewer::OnGuiRender()
    {

    }

    void ModelViewer::OnClose()
    {

    }
}