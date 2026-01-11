#include "MultiWindow.h"

namespace TestApp
{
    void MultiWindow::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        pxl::WindowSpecs customSpecs = {};
        customSpecs.RendererAPI = pxl::RendererAPIType::None;
        customSpecs.Title = "pxlFramework Test App - Window Test - Window A";
        customSpecs.WindowMode = pxl::WindowMode::Windowed;
        m_TestWindowA = pxl::Window::Create(customSpecs);

        customSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;
        customSpecs.Title = "pxlFramework Test App - Window Test - Window B";
        customSpecs.Size = { 800, 600 };
        m_TestWindowB = pxl::Window::Create(customSpecs);

        pxl::Renderer::Init(m_TestWindowB);
        pxl::Input::Init(m_TestWindowB);
        pxl::GUI::Init(m_TestWindowB);

        pxl::Renderer::SetClearColour({ 0.5f, 0.5f, 0.7f, 1.0f });
    }

    void MultiWindow::OnUpdate(float dt)
    {
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Escape))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftAlt) && pxl::Input::IsKeyPressed(pxl::KeyCode::Enter))
            m_TestWindowB->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num1))
            m_TestWindowA->SetMonitor(pxl::Window::GetPrimaryMonitor());

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num2))
            m_TestWindowA->SetMonitor(1);
    }
}