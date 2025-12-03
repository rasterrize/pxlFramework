#include "LinesTest.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_Window = nullptr;
    static std::shared_ptr<pxl::PerspectiveCamera> s_Camera = nullptr;

    static constexpr int64_t RPCCLIENT_ID = 1141683223064231946;

    static glm::vec3 s_LineRotation = glm::vec3(0.0f);

    void LinesTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);

        pxl::Renderer::SetClearColour({ 0.078f, 0.094f, 0.109f, 1.0f });

        s_Camera = pxl::Camera::CreatePerspective({
            .FOV = 45.0f,
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = 0.01f,
            .FarClip = 1000.0f,
        });

        s_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::Line, s_Camera);

        //std::string rendererAPIString = pxl::EnumStringHelper::ToString(windowSpecs.RendererAPI);

        // pxl::DiscordRPC::Init(RPCCLIENT_ID);
        // pxl::DiscordRPC::SetPresence({ RPCCLIENT_ID, "Test App", "Running test 'LinesTest' utilizing " + rendererAPIString, pxl::DiscordRPCActivityType::Playing, "ta"});
    }

    void LinesTest::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        auto cameraPosition = s_Camera->GetPosition();
        auto cameraFOV = s_Camera->GetFOV();
        auto cameraSpeed = 2.0f;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Escape))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftAlt) && pxl::Input::IsKeyPressed(pxl::KeyCode::Enter))
            s_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::F7))
            s_Window->GetGraphicsContext()->ToggleVSync();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftShift))
        {
            cameraSpeed *= 3.0f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftControl))
        {
            cameraSpeed *= 0.5f;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::W))
        {
            cameraPosition.y += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::A))
        {
            cameraPosition.x -= cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::S))
        {
            cameraPosition.y -= cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::D))
        {
            cameraPosition.x += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::Q))
        {
            cameraPosition.z += cameraSpeed * dt;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::E))
        {
            cameraPosition.z -= cameraSpeed * dt;
        }

        if (pxl::Input::IsMouseScrolledUp())
        {
            cameraFOV -= cameraSpeed * 0.5f;
        }

        if (pxl::Input::IsMouseScrolledDown())
        {
            cameraFOV += cameraSpeed * 0.5f;
        }

        s_LineRotation.z += dt * 20.0f;

        s_Camera->SetPosition(cameraPosition);
        s_Camera->SetFOV(cameraFOV);
    }

    void LinesTest::OnRender()
    {
        PXL_PROFILE_SCOPE;

        pxl::Renderer::AddLine({ -0.5f, 1.0f, 1.0f }, { 0.5f, -1.0f, -1.0f }, s_LineRotation, { 0.4f, 0.8f, 0.2f, 1.0f });
    }

    void LinesTest::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void LinesTest::OnClose()
    {
    }

    std::shared_ptr<pxl::Window> LinesTest::GetWindow()
    {
        return s_Window;
    }
}