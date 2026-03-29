#include "QuadsTest.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Texture> s_StoneTexture = nullptr;
    static std::shared_ptr<pxl::Texture> s_CursorTexture = nullptr;

    static pxl::Quad s_DynamicQuad;
    static pxl::Quad s_TexturedDynamicQuad;

    static pxl::Quad* selectedQuad = nullptr;

    static std::vector<pxl::Quad*> quads;

    void QuadsTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        m_Window = pxl::Window::Create(windowSpecs);

        pxl::RendererConfig rendererConfig;
        rendererConfig.Window = m_Window;
        rendererConfig.ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };
        rendererConfig.VerticalSync = false;
        pxl::Application::Get().InitRenderer(rendererConfig);

        pxl::Input::Init(m_Window);

        m_Camera = pxl::Camera::CreateOrthographic({
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

        // s_StoneTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/stone.png", { .Filter = pxl::SampleFilter::Nearest });
        // s_CursorTexture = pxl::FileSystem::LoadTextureFromImage("assets/textures/cursor@2x.png", { .Filter = pxl::SampleFilter::Nearest });
        // s_TextureAtlas = pxl::FileSystem::LoadTextureFromImage("assets/textures/atlas.png", { .Filter = pxl::SampleFilter::Nearest });

        // clang-format off
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
            // .Texture  = s_TextureAtlas,
        };
        // clang-format on

        quads.push_back(&s_DynamicQuad);
        quads.push_back(&s_TexturedDynamicQuad);
    }

    void QuadsTest::OnUpdate(float dt)
    {
        auto cameraPosition = m_Camera->GetPosition();
        auto cameraRotation = m_Camera->GetRotation();
        auto cameraSpeed = 2.0f;

        auto windowSize = m_Window->GetSize();
        m_Camera->SetRight(static_cast<float>(windowSize.Width));
        m_Camera->SetTop(static_cast<float>(windowSize.Height));

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::Escape))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::LeftAlt) && pxl::Input::IsKeyPressed(pxl::KeyCode::Enter))
            m_Window->NextWindowMode();

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::F7))
        // m_Window->GetGraphicsContext()->ToggleVSync();

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num1))
        //     s_DynamicQuad.Origin = pxl::Origin2D::TopLeft;

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num2))
        //     s_DynamicQuad.Origin = pxl::Origin2D::TopRight;

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num3))
        //     s_DynamicQuad.Origin = pxl::Origin2D::BottomLeft;

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num4))
        //     s_DynamicQuad.Origin = pxl::Origin2D::BottomRight;

        // if (pxl::Input::IsKeyPressed(pxl::KeyCode::Num5))
        //     s_DynamicQuad.Origin = pxl::Origin2D::Center;

        // if (pxl::Input::IsMouseButtonPressed(pxl::MouseCode::LeftButton))
        // {
        //     bool foundQuad = false;
        //     for (auto quad : quads)
        //     {
        //         if (quad->Contains(s_CursorPosition))
        //         {
        //             selectedQuad = quad;
        //             foundQuad = true;
        //             break;
        //         }
        //     }
        // }

        // m_Camera->SetPosition({ cameraPosition.x, cameraPosition.y, cameraPosition.z });

        s_DynamicQuad.Rotation.z += 25.0f * dt;
        s_TexturedDynamicQuad.Rotation.z -= 25.0f * dt;
    }

    void QuadsTest::OnRender(pxl::Renderer& renderer)
    {
        renderer.Submit(s_DynamicQuad);
        renderer.Submit(s_TexturedDynamicQuad);
    }

    void QuadsTest::OnGUIRender()
    {
#ifdef PXL_ENABLE_IMGUI
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
#endif
    }
}