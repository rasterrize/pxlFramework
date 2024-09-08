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
    static std::shared_ptr<pxl::Texture> s_CursorTexture = nullptr;
    static std::shared_ptr<pxl::Texture> s_TextureAtlas = nullptr;

    static pxl::Quad s_StaticQuad;
    static pxl::Quad s_TexturedStaticQuad;
    static pxl::Quad s_DynamicQuad;
    static pxl::Quad s_TexturedDynamicQuad;
    static pxl::Quad s_CursorQuad;

    static glm::vec2 s_CursorPosition = glm::vec2(0.0f);

    void QuadsTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        windowSpecs.Title += " - Running Test 'QuadsTest'";
        windowSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;

        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);
        pxl::GUI::Init(s_Window);

        pxl::Renderer::SetClearColour(s_ClearColour);
        
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

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::Quad, s_Camera);

        s_StoneTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/stone.png");
        s_CursorTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/cursor@2x.png");
        s_TextureAtlas = pxl::FileSystem::LoadTextureFromImage("assets/textures/atlas.png");

        s_StaticQuad = {
            .Position = { 200.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0, 45.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = pxl::Colour::GetColourAsVec4(pxl::ColourName::Yellow),
            .Origin   = pxl::Origin2D::BottomRight,
        };

        s_TexturedStaticQuad = {
            .Position = { 400.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0, 0.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = glm::vec4(1.0f),
            .Origin   = pxl::Origin2D::TopLeft,
            //.Texture = s_StoneTexture,
        };

        s_DynamicQuad = {
            .Position = { 600.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0f, 0.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = glm::vec4(1.0f, 0.4f, 0.5f, 1.0f),
            .Origin   = pxl::Origin2D::Center,
        };

        s_TexturedDynamicQuad = {
            .Position = { 800.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0f, 0.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = glm::vec4(1.0f),
            .Origin   = pxl::Origin2D::Center,
            .Texture  = s_TextureAtlas,
        };

        s_CursorQuad = {
            .Position = { 0.0f, 0.0f, 1.0f },
            .Rotation = glm::vec3(0.0f),
            .Size     = glm::vec2(75.0f),
            .Colour   = glm::vec4(1.0f),
            .Origin   = pxl::Origin2D::Center,
            .Texture = s_CursorTexture,
        };

        pxl::Renderer::AddStaticQuad(s_StaticQuad);

        pxl::Renderer::AddStaticQuad(s_TexturedStaticQuad);

        pxl::Renderer::StaticGeometryReady();

        //pxl::Input::SetCursorVisibility(false);

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

        s_CursorQuad.Position = glm::vec3(s_CursorPosition, 1.0f);

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            s_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            s_Window->ToggleVSync();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_1))
            s_DynamicQuad.Origin = pxl::Origin2D::TopLeft;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_2))
            s_DynamicQuad.Origin = pxl::Origin2D::TopRight;
        
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_3))
            s_DynamicQuad.Origin = pxl::Origin2D::BottomLeft;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_4))
            s_DynamicQuad.Origin = pxl::Origin2D::BottomRight;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_5))
            s_DynamicQuad.Origin = pxl::Origin2D::Center;

        if (pxl::Input::IsMouseButtonPressed(pxl::MouseCode::PXL_MOUSE_BUTTON_LEFT))
        {
            if (s_DynamicQuad.Contains(s_CursorPosition))
                APP_LOG_INFO("Dynamic quad pressed!");

            if (s_TexturedDynamicQuad.Contains(s_CursorPosition))
                APP_LOG_INFO("Textured dynamic quad pressed!");
        }

        s_Camera->SetPosition({ cameraPosition.x, cameraPosition.y, cameraPosition.z });

        s_DynamicQuad.Rotation.z += 25.0f * dt;
        s_TexturedDynamicQuad.Rotation.z -= 25.0f * dt;
    }

    void QuadsTest::OnRender()
    {
        // Draw other quads
        pxl::Renderer::AddQuad(s_DynamicQuad);
        pxl::Renderer::AddQuad(s_TexturedDynamicQuad);

        // Draw cursor quad
        // NOTE: this must be done last
        pxl::Renderer::AddQuad(s_CursorQuad);

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