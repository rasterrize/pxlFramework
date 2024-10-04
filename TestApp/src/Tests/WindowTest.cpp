#include "WindowTest.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_TestWindowA = nullptr;
    static std::shared_ptr<pxl::Window> s_TestWindowB = nullptr;
    
    void WindowTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.RendererAPI = pxl::RendererAPIType::None;
        windowSpecs.Title = "pxlFramework Test App - Window Test - Window A";
        windowSpecs.Size = { 1280, 720 };
        s_TestWindowA = pxl::Window::Create(windowSpecs);

        windowSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;
        windowSpecs.Title = "pxlFramework Test App - Window Test - Window B";
        windowSpecs.Size = { 800, 600 };
        s_TestWindowB = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_TestWindowB);
        pxl::Input::Init(s_TestWindowB);
        pxl::GUI::Init(s_TestWindowB);

        pxl::Renderer::SetClearColour({ 0.5f, 0.5f, 0.7f, 1.0f });
    }

    void WindowTest::OnUpdate(float dt)
    {
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }
        
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_1))
            s_TestWindowA->SetMonitor(pxl::Window::GetPrimaryMonitor());

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_2))
            s_TestWindowA->SetMonitor(pxl::Window::GetMonitors()[1]);
    }

    void WindowTest::OnRender()
    {
    }

    void WindowTest::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void WindowTest::OnClose()
    {
    }
}