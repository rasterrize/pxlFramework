#include "QuadsTest.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_Window = nullptr;
    static std::shared_ptr<pxl::OrthographicCamera> s_Camera = nullptr;

    static glm::vec4 s_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

    static bool s_ControllingCamera = true;

    static glm::vec2 s_SelectedTile = glm::vec2(0.0f);

    static uint32_t s_BlueQuadAmount = 10;
    static uint32_t s_OrangeQuadAmount = 10;

    static std::shared_ptr<pxl::Texture> s_StoneTexture = nullptr;

    static pxl::Quad s_MainQuad;

    static glm::dvec2 s_CursorPosition = glm::dvec2(0.0f);

    void QuadsTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'QuadsTest'";

        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);
        
        s_Camera = pxl::OrthographicCamera::Create({
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = -10.0f,
            .FarClip = 10.0f,
            .Zoom = 1.0f,
            .Left = 0.0f,
            .Right = static_cast<float>(windowSpecs.Width),
            .Bottom = 0.0f,
            .Top = static_cast<float>(windowSpecs.Height),
            .UseAspectRatio = false,
        });

        pxl::Renderer::SetClearColour(s_ClearColour);

        pxl::Renderer::SetQuadCamera(s_Camera);

        s_StoneTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/stone.png");

        s_MainQuad = {
            .Position = { 150.0f, 150.0f, 0.0f },
            .Rotation = glm::vec3(0.0f),
            .Size = glm::vec2(150.0f),
            .Colour = { 1.0f, 1.0f, 0.0f, 1.0f },
            .Origin = pxl::Origin::BottomLeft,
        };

        pxl::GUI::Init(s_Window);
    }

    void QuadsTest::OnUpdate(float dt)
    {
        auto cameraPosition = s_Camera->GetPosition();
        auto cameraRotation = s_Camera->GetRotation();
        auto cameraSpeed = 2.0f;

        auto windowSize = s_Window->GetSize();
        s_Camera->SetRight(static_cast<float>(windowSize.Width));
        s_Camera->SetTop(static_cast<float>(windowSize.Height));
        
        s_CursorPosition = pxl::Input::GetCursorPosition();

        // Flip cursor position to align with pxlFramework cartesian plane
        s_CursorPosition.y = windowSize.Height - s_CursorPosition.y;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            s_Window->ToggleVSync();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_SHIFT))
            cameraSpeed *= 3.0f;

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

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_1))
            s_MainQuad.Origin = pxl::Origin::TopLeft;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_2))
            s_MainQuad.Origin = pxl::Origin::TopRight;
        
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_3))
            s_MainQuad.Origin = pxl::Origin::BottomLeft;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_4))
            s_MainQuad.Origin = pxl::Origin::BottomRight;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_5))
            s_MainQuad.Origin = pxl::Origin::Center;

        if (s_MainQuad.Contains(glm::vec2(s_CursorPosition)) && pxl::Input::IsMouseButtonPressed(pxl::MouseCode::PXL_MOUSE_BUTTON_LEFT))
        {
            APP_LOG_INFO("Quad pressed!");
        }

        // if (pxl::Input::IsMouseScrolledUp())
        //     cameraZoom -= cameraSpeed * 0.5f;

        // if (pxl::Input::IsMouseScrolledDown())
        //     cameraZoom += cameraSpeed * 0.5f;

        s_Camera->SetPosition({ cameraPosition.x, cameraPosition.y, cameraPosition.z });
    }

    void QuadsTest::OnRender()
    {
        pxl::Renderer::Clear();

        pxl::Renderer::AddQuad(s_MainQuad);

        // for (uint32_t x = 0; x < s_BlueQuadAmount; x += 2)
        // {
        //     for (uint32_t y = 0; y < s_BlueQuadAmount; y += 2)
        //     {
        //         pxl::Renderer::AddQuad({ x - (s_BlueQuadAmount / 2.0f), y - (s_BlueQuadAmount / 2.0f), 1.0f }, glm::vec3(0.0f), glm::vec2(1.0f), { 0.4f, 0.4f, 0.7f, 1.0f });
        //     }
        // }

        //pxl::Renderer::AddQuad({ -0.5f, -0.5f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), { 1.0f, 0.5f, 0.3f, 1.0f });
        //pxl::Renderer::AddQuad({ 2.0f, 2.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(0.5f), { 1.0f, 0.5f, 0.3f, 1.0f });
        // pxl::Renderer::AddTexturedQuad({ 0.0f, 0.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), s_StoneTexture);
        // pxl::Renderer::AddTexturedQuad({ 2.0f, 2.0f, 0.0f }, glm::vec3(0.0f), glm::vec3(1.0f), s_StoneTexture);

        //pxl::Renderer::AddQuad(glm::vec3(0.0f));
    }

    void QuadsTest::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        ImGui::Begin("QuadsTest");
        ImGui::Text("Cursor Position: %f, %f", s_CursorPosition.x, s_CursorPosition.y);
        ImGui::End();
    }

    void QuadsTest::OnClose()
    {
    }
}