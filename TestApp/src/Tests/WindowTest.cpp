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
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_1))
            m_TestWindowA->SetMonitor(pxl::Window::GetPrimaryMonitor());

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_2))
            m_TestWindowA->SetMonitor(pxl::Window::GetMonitors()[1]);
    }

    void WindowTest::OnRender()
    {
        pxl::Renderer::Clear();
    }

    void WindowTest::OnImGuiRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void WindowTest::OnClose()
    {
    }
}