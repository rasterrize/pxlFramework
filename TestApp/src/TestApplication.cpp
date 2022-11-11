#include "TestApplication.h"
#include <pxl.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        pxl::Window::Init(1280, 720, "pxlFramework", pxl::RendererAPI::OpenGL);
        pxl::Input::Init();

        pxl::Renderer::SetClearColour(0.2, 0.5, 0.4, 1.0);
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate()
    {
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_L))
        {
            pxl::Logger::Log(pxl::LogLevel::Info, "L");
        }

        //pxl::Renderer::Clear();

        //pxl::Renderer::Draw();
    }
}