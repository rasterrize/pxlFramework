#include "TestApplication.h"
#include <pxl.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        pxl::Window::Init(1280, 720, "pxlFramework", pxl::RendererAPI::OpenGL);
        pxl::Input::Init();
        pxl::Camera::Init(pxl::CameraType::Orthographic);

        pxl::Renderer::SetClearColour(0.2, 0.5, 0.4, 1.0);
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate()
    {
        auto cameraPosition = pxl::Camera::GetPosition();
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_W))
        {
            cameraPosition.y += 0.1f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_A))
        {
            cameraPosition.x -= 0.1f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_S))
        {
            cameraPosition.y -= 0.1f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_D))
        {
            cameraPosition.x += 0.1f;
        }
        
        pxl::Camera::SetPosition(glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));

        //pxl::Renderer::Clear();

        //pxl::Renderer::Draw();
    }
}