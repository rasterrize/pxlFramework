#include "TestApplication.h"
#include <pxl.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        pxl::Window::Init(1280, 720, "pxlFramework");
        pxl::Input::Init();
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate()
    {
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_L))
        {
            pxl::Logger::Log(pxl::LogLevel::Info, "L");
            pxl::Input::Shutdown();
        }
    }
}