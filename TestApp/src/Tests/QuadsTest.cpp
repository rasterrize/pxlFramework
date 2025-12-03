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

    static pxl::Quad* selectedQuad = nullptr;

    static std::vector<pxl::Quad*> quads;

    static glm::vec2 s_CursorPosition = glm::vec2(0.0f);

    void QuadsTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        // Override renderer API
        if (windowSpecs.RendererAPI != pxl::RendererAPIType::OpenGL)
        {
            auto enumString = pxl::EnumStringHelper::ToString(windowSpecs.RendererAPI);
            windowSpecs.Title.replace(windowSpecs.Title.find(enumString), enumString.size(), std::string("OpenGL"));
            windowSpecs.RendererAPI = pxl::RendererAPIType::OpenGL;
        }

        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);
        pxl::GUI::Init(s_Window);

        pxl::Renderer::SetClearColour(s_ClearColour);

        s_Camera = pxl::Camera::CreateOrthographic({
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = -10.0f,
            .FarClip = 10.0f,
            .Zoom = 1.0f,
            .Left = 0.0f,
            .Right = static_cast<float>(windowSpecs.Size.Width),
            .Bottom = 0.0f,
            .Top = static_cast<float>(windowSpecs.Size.Height),
            .UseAspectRatio = false,
        });

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::Quad, s_Camera);

        s_StoneTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/stone.png", { .Filter = pxl::SampleFilter::Nearest });
        s_CursorTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/cursor@2x.png", { .Filter = pxl::SampleFilter::Nearest });
        s_TextureAtlas = pxl::FileSystem::LoadTextureFromImage("assets/textures/atlas.png", { .Filter = pxl::SampleFilter::Nearest });

        // clang-format off
        s_StaticQuad = {
            .Position = { 400.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0, 45.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = pxl::Colour::AsVec4(pxl::ColourName::Pink),
            .Origin   = pxl::Origin2D::BottomRight,
        };

        s_TexturedStaticQuad = {
            .Position = { 600.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0, 0.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = glm::vec4(1.0f),
            .Origin   = pxl::Origin2D::TopLeft,
            //.Texture = s_StoneTexture,
        };

        s_DynamicQuad = {
            .Position = { 800.0f, 450.0f, 0.0f },
            .Rotation = { 0.0f, 0.0f, 0.0f },
            .Size     = glm::vec2(75.0f),
            .Colour   = glm::vec4(1.0f, 0.4f, 0.5f, 1.0f),
            .Origin   = pxl::Origin2D::Center,
        };

        s_TexturedDynamicQuad = {
            .Position = { 1000.0f, 450.0f, 0.0f },
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
        // clang-format on

        quads.push_back(&s_StaticQuad);
        quads.push_back(&s_TexturedStaticQuad);
        quads.push_back(&s_DynamicQuad);
        quads.push_back(&s_TexturedDynamicQuad);

        pxl::Renderer::AddStaticQuad(s_StaticQuad);
        pxl::Renderer::AddStaticQuad(s_TexturedStaticQuad);

        pxl::Renderer::StaticGeometryReady();
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

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Escape))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftAlt) && pxl::Input::IsKeyPressed(pxl::KeyCode::Enter))
            s_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::F7))
            s_Window->GetGraphicsContext()->ToggleVSync();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num1))
            s_DynamicQuad.Origin = pxl::Origin2D::TopLeft;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num2))
            s_DynamicQuad.Origin = pxl::Origin2D::TopRight;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num3))
            s_DynamicQuad.Origin = pxl::Origin2D::BottomLeft;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num4))
            s_DynamicQuad.Origin = pxl::Origin2D::BottomRight;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num5))
            s_DynamicQuad.Origin = pxl::Origin2D::Center;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::T))
            s_Window->SetPosition(200, -2000);

        if (pxl::Input::IsMouseButtonPressed(pxl::MouseCode::LeftButton))
        {
            bool foundQuad = false;
            for (auto quad : quads)
            {
                if (quad->Contains(s_CursorPosition))
                {
                    selectedQuad = quad;
                    foundQuad = true;
                    break;
                }
            }
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
        // NOTE: this must be done last for transparency to work
        pxl::Renderer::AddQuad(s_CursorQuad);
    }

    void QuadsTest::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        if (selectedQuad)
        {
            ImGui::SetNextWindowPos({ 400.0f, 50.0f }, ImGuiCond_Once);
            ImGui::SetNextWindowSize({ 230.0f, 150.0f }, ImGuiCond_Once);
            ImGui::Begin("Quad Settings");

            ImGui::DragFloat3("Position", glm::value_ptr(selectedQuad->Position));
            ImGui::DragFloat3("Rotation", glm::value_ptr(selectedQuad->Rotation));
            ImGui::DragFloat2("Size", glm::value_ptr(selectedQuad->Size));
            ImGui::ColorEdit4("Colour", glm::value_ptr(selectedQuad->Colour));

            ImGui::End();
        }
    }

    void QuadsTest::OnClose()
    {
    }

    std::shared_ptr<pxl::Window> QuadsTest::GetWindow()
    {
        return s_Window;
    }
}