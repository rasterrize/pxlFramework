#include "OGLVK.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_Window;
    static std::shared_ptr<pxl::PerspectiveCamera> s_Camera;

    static constexpr int64_t RPCCLIENT_ID = 1141683223064231946;

    static float rotation = 0.0f;
    static float rotation2 = 0.0f;

    void OGLVK::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);

        pxl::Renderer::SetClearColour({ 0.078f, 0.094f, 0.109f, 1.0f });

        s_Camera = pxl::Camera::CreatePerspective({
            .FOV = 45.0f,
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = 1.0f,
            .FarClip = 1000.0f,
        });

        s_Camera->SetPosition({ 0.0f, -0.2f, 15.0f });

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::All, s_Camera);

        auto rendererAPIString = pxl::EnumStringHelper::ToString(windowSpecs.RendererAPI);

        // pxl::DiscordRPC::Init(RPCCLIENT_ID);
        // pxl::DiscordRPC::SetPresence({ RPCCLIENT_ID, "Test App", "Running test 'OGLVK' utilizing " + rendererAPIString, pxl::DiscordRPCActivityType::Playing, "ta"});

#ifdef TA_DEBUG
        //pxl::GUI::Init(m_Window);
#endif
    }

    void OGLVK::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        auto cameraPosition = s_Camera->GetPosition();
        auto cameraFOV = s_Camera->GetFOV();
        auto cameraSpeed = 2.0f;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            s_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            s_Window->GetGraphicsContext()->ToggleVSync();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
            cameraSpeed *= 3.0f;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_CONTROL))
            cameraSpeed *= 0.5f;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
            cameraPosition.y += cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_A))
            cameraPosition.x -= cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
            cameraPosition.y -= cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_D))
            cameraPosition.x += cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_Q))
            cameraPosition.z += cameraSpeed * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_E))
            cameraPosition.z -= cameraSpeed * dt;

        if (pxl::Input::IsMouseScrolledUp())
            cameraFOV -= cameraSpeed * 0.5f;

        if (pxl::Input::IsMouseScrolledDown())
            cameraFOV += cameraSpeed * 0.5f;

        s_Camera->SetPosition(cameraPosition);
        s_Camera->SetFOV(cameraFOV);
        s_Camera->SetAspectRatio(s_Window->GetAspectRatio());

        rotation += 20.0f * dt;
        rotation2 -= 20.0f * dt;
    }

    void OGLVK::OnRender()
    {
        PXL_PROFILE_SCOPE;

        constexpr uint32_t objectCount = 10;
        constexpr uint32_t gridSize = 5;

        for (uint32_t x = 0; x < objectCount; x += 2)
        {
            for (uint32_t y = 0; y < objectCount; y += 2)
            {
                pxl::Renderer::AddQuad({ x - (objectCount / 2.0f) - 0.2f, y - (objectCount / 2.0f) - 0.2f, 0.0f }, { 0.0f, 0.0f, rotation }, glm::vec2(1.4f), glm::vec4(1.0f));
                pxl::Renderer::AddQuad({ x - (objectCount / 2.0f) - 0.1f, y - (objectCount / 2.0f) - 0.1f, 0.1f }, glm::vec3(0.0f), glm::vec2(1.2f), { 0.8f, 0.5f, 0.3f, 1.0f });
                pxl::Renderer::AddQuad({ x - (objectCount / 2.0f), y - (objectCount / 2.0f), 0.2f }, { 0.0f, 0.0f, rotation2 }, glm::vec2(1.0f), { 0.4f, 0.4f, 0.7f, 1.0f });
                pxl::Renderer::AddCube({ x - (objectCount / 2.0f), y - (objectCount / 2.0f), 1.0f }, { 0.0f, rotation2, rotation2 }, glm::vec3(1.0f), { 0.4f, 0.8f, 0.2f, 1.0f });
            }
        }

        //pxl::Renderer::AddLine({ -10.0, 10.0, 1.0f }, { 10.0, -10.0, 1.0f }, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(1.0f));

        // {
        //     for (uint32_t x = 0; x < gridSize; x++)
        //     {
        //         pxl::Renderer::AddLine({ x - gridSize, 0.0, gridSize }, { -10.0, 0.0, -10.0f }, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec4(1.0f));
        //     }

        //     for (uint32_t y = 0; y < gridSize; y++)
        //     {

        //     }
        // }
    }

    void OGLVK::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        ImGui::ShowDemoWindow();
    }

    void OGLVK::OnClose()
    {
    }
}