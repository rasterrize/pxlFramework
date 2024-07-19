#include "ModelViewer.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> ModelViewer::m_Window;
    std::shared_ptr<pxl::Camera> ModelViewer::m_Camera;
    glm::vec4 ModelViewer::m_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

    void ModelViewer::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'ModelViewer'";
        windowSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;

        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        pxl::Renderer::SetClearColour(m_ClearColour);

        m_Camera = pxl::Camera::Create({ pxl::ProjectionType::Orthographic, 16.0f / 9.0f, -10.0, 10.0f });

        pxl::Renderer::AddStaticQuad({-0.5f, -0.5f, 0.0f});
        pxl::Renderer::StaticGeometryReady();
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
    }
        
    void ModelViewer::OnRender()
    {
        PXL_PROFILE_SCOPE;
        
        pxl::Renderer::Clear();
    
        pxl::Renderer::DrawStaticQuads();
    }

    void ModelViewer::OnClose()
    {

    }
}