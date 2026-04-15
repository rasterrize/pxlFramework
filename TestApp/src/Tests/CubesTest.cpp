#include "CubesTest.h"

namespace TestApp
{
    static glm::vec4 s_ClearColour = glm::vec4(0.5f, 0.3f, 0.6f, 1.0f);

    static glm::vec3 s_PlayerPosition = glm::vec3(0.0f);

    static bool s_ControllingCamera = true;

    static glm::vec3 s_CubeRotation = glm::vec3(0.0f);

    void CubesTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Input::Init(m_Window);

        // m_Camera = pxl::Camera::CreatePerspective({
        //     .FOV = 45.0f,
        //     .AspectRatio = 16.0f / 9.0f,
        //     .NearClip = 0.1f,
        //     .FarClip = 1000.0f,
        // });

        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });
    }

    void CubesTest::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        auto cameraPosition = m_Camera->GetPosition();
        auto cameraRotation = m_Camera->GetRotation();
        auto cameraFOV = m_Camera->GetFOV();
        auto cameraSpeed = 2.0f;
        auto cursorPos = pxl::Input::GetCursorPosition();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Tab))
        {
            s_ControllingCamera = !s_ControllingCamera;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftShift))
        {
            cameraSpeed *= 5.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::W))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.y += cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::A))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.x -= cameraSpeed * dt;
            }
            else
            {
                s_PlayerPosition.x -= 1.0f * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::S))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.y -= cameraSpeed * dt;
            }
        }
        if (pxl::Input::IsKeyHeld(pxl::KeyCode::D))
        {
            if (s_ControllingCamera)
            {
                cameraPosition.x += cameraSpeed * dt;
            }
            else
            {
                s_PlayerPosition.x += 1.0f * dt;
            }
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::Q))
        {
            cameraPosition.z -= cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::E))
        {
            cameraPosition.z += cameraSpeed * dt;
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraFOV -= cameraSpeed * 0.5f;
        }
        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraFOV += cameraSpeed * 0.5f;
        }

        m_Camera->SetPosition({ cameraPosition.x, cameraPosition.y, cameraPosition.z });
        m_Camera->SetFOV(cameraFOV);

        s_CubeRotation.x += 60.0f * dt;
        s_CubeRotation.y += 60.0f * dt;
        s_CubeRotation.z += 60.0f * dt;
    }

    void CubesTest::OnRender(pxl::Renderer& renderer)
    {
        PXL_PROFILE_SCOPE;

    //     for (int32_t x = -5; x < 10; x += 2)
    //     {
    //         for (int32_t y = -5; y < 10; y += 2)
    //         {
    //             for (int32_t z = -5; z < 10; z += 2)
    //             {
    //                 pxl::Renderer::AddCube(glm::vec3(x, y, z), s_CubeRotation, glm::vec3(1.0f), glm::vec4(0.8f, 0.5f, 0.3f, 1.0f));
    //             }
    //         }
    //     }

    //     //pxl::Renderer::AddCube(glm::vec3(0.0f), s_CubeRotation, glm::vec3(1.0f), glm::vec4(0.8f, 0.5f, 0.3f, 1.0f));
    }

    void CubesTest::OnGUIRender()
    {
#ifdef PXL_ENABLE_IMGUI

#endif
    }
}