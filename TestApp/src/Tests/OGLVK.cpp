#include "OGLVK.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> OGLVK::m_Window;
    std::shared_ptr<pxl::Shader> OGLVK::m_Shader; 
    std::shared_ptr<pxl::GraphicsPipeline> OGLVK::m_Pipeline;

    void OGLVK::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'OGLVK'";
        
        m_Window = pxl::Window::Create(windowSpecs);
        
        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        pxl::Renderer::SetClearColour({ 0.078f, 0.094f, 0.109f, 1.0f });

        if (windowSpecs.RendererAPI == pxl::RendererAPIType::OpenGL)
        {
            m_Shader = pxl::FileLoader::LoadGLSLShader("assets/shaders/first.vert", "assets/shaders/first.frag");
        }
        else if (windowSpecs.RendererAPI == pxl::RendererAPIType::Vulkan)
        {
            auto vertBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/vert.spv");
            auto fragBin = pxl::FileLoader::LoadSPIRV("assets/shaders/compiled/frag.spv");
            m_Shader = pxl::Shader::Create(vertBin, fragBin);
        }

        pxl::BufferLayout layout;
        layout.Add(pxl::BufferDataType::Float3, false); // vertex position
        layout.Add(pxl::BufferDataType::Float4, false); // colour
        layout.Add(pxl::BufferDataType::Float2, false); // texture coords

        m_Pipeline = pxl::GraphicsPipeline::Create(m_Shader, layout);

        pxl::Renderer::AddStaticQuad({ -0.5f, -0.5f, 0.0f });
        pxl::Renderer::StaticGeometryReady();
    }

    void OGLVK::OnUpdate(float dt)
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
        
    void OGLVK::OnRender()
    {
        pxl::Renderer::Clear();

        pxl::Renderer::Submit(m_Pipeline);
        
        pxl::Renderer::DrawStaticQuads();
    }

    void OGLVK::OnClose()
    {

    }
}