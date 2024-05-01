#include "ModelViewer.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> ModelViewer::m_Window;
    std::shared_ptr<pxl::Camera> ModelViewer::m_Camera;
    std::shared_ptr<pxl::Shader> ModelViewer::m_Shader; 
    std::shared_ptr<pxl::GraphicsPipeline> ModelViewer::m_Pipeline;
    glm::vec4 ModelViewer::m_ClearColour;

    void ModelViewer::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'ModelViewer'";
        windowSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;

        m_Window = pxl::Window::Create(windowSpecs);
        APP_ASSERT(m_Window);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        m_ClearColour = glm::vec4(0.078f, 0.094f, 0.109f, 1.0f);
        pxl::Renderer::SetClearColour(m_ClearColour);

        m_Camera = pxl::Camera::Create(pxl::CameraType::Orthographic, { 16.0f / 9.0f, -10.0, 10.0f });

        auto windowRendererAPI = m_Window->GetWindowSpecs().RendererAPI;

        if (windowRendererAPI == pxl::RendererAPIType::OpenGL)
        {
            m_Shader = pxl::FileLoader::LoadGLSLShader("assets/shaders/first.vert", "assets/shaders/first.frag");
        }
        else if (windowRendererAPI == pxl::RendererAPIType::Vulkan)
        {
            auto vertBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/vert.spv");
            auto fragBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/frag.spv");
            m_Shader = pxl::Shader::Create(vertBin, fragBin);
        }

        APP_LOG_INFO("Compiling shaders... (1/1)");

        pxl::BufferLayout layout;
        layout.Add(pxl::BufferDataType::Float3, false); // vertex position
        layout.Add(pxl::BufferDataType::Float4, false); // colour
        layout.Add(pxl::BufferDataType::Float2, false); // texture coords

        m_Pipeline = pxl::GraphicsPipeline::Create(m_Shader, layout);

        pxl::Renderer::AddStaticQuad({-0.5f, -0.5f, 0.0f});
        pxl::Renderer::StaticGeometryReady();
    }

    void ModelViewer::OnUpdate(float dt)
    {
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
        pxl::Renderer::Clear();

        pxl::Renderer::Submit(m_Pipeline);
    
        pxl::Renderer::DrawStaticQuads();
    }

    void ModelViewer::OnClose()
    {

    }
}