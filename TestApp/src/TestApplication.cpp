#include "TestApplication.h"

namespace TestApp
{
    TestApplication::TestApplication()
    {

        pxl::Window::Init(1280, 720, "pxlFramework", pxl::RendererAPI::OpenGL);
        pxl::Input::Init();
        pxl::Camera::Init(pxl::CameraType::Perspective);
        pxl::Camera::SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

        pxl::Renderer::SetClearColour(0.2, 0.5, 0.4, 1.0);
    }

    TestApplication::~TestApplication()
    {

    }

    void TestApplication::OnUpdate()
    {
        auto cameraPosition = pxl::Camera::GetPosition();
        pxl::Logger::LogWarn(std::to_string(cameraPosition.x) + ", " + std::to_string(cameraPosition.y) + ", " + std::to_string(cameraPosition.z));
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_W))
        {
            cameraPosition.y += 0.01f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_A))
        {
            cameraPosition.x -= 0.01f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_S))
        {
            cameraPosition.y -= 0.01f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_D))
        {
            cameraPosition.x += 0.01f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_Q))
        {
            cameraPosition.z -= 0.01f;
        }
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_E))
        {
            cameraPosition.z += 0.01f;
        }

        pxl::Camera::SetPosition(glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));

        //pxl::Renderer::Clear();

        //pxl::Renderer::Draw();
    }
}