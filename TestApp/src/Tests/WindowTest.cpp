#include "WindowTest.h"

namespace TestApp
{
    std::shared_ptr<pxl::Window> WindowTest::m_TestWindowA;
    std::shared_ptr<pxl::Window> WindowTest::m_TestWindowB;
    
    void WindowTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.RendererAPI = pxl::RendererAPIType::None;
        windowSpecs.Title = "pxlFramework Test App - Window Test - Window A";
        windowSpecs.Width = 1280;
        windowSpecs.Height = 720;
        m_TestWindowA = pxl::Window::Create(windowSpecs);

        windowSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;
        windowSpecs.Title = "pxlFramework Test App - Window Test - Window B";
        windowSpecs.Width = 800;
        windowSpecs.Height = 600;
        m_TestWindowB = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_TestWindowB);
        pxl::Renderer::SetClearColour({ 0.5f, 0.5f, 0.7f, 1.0f });
    }

    void WindowTest::OnUpdate(float dt)
    {
    }

    void WindowTest::OnRender()
    {
        pxl::Renderer::Clear();
    }

    void WindowTest::OnImGuiRender()
    {
    }

    void WindowTest::OnClose()
    {
    }
}