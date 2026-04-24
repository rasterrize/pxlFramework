#include "QuadsTest.h"

using namespace pxl;

namespace TestApp
{
    void QuadsTest::OnStart(WindowSpecs& windowSpecs)
    {
        m_Window = Window::Create(windowSpecs);

        RendererConfig rendererConfig = {};
        rendererConfig.Window = m_Window;
        rendererConfig.ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };
        rendererConfig.VerticalSync = false;
        rendererConfig.AllowTearing = true;
        rendererConfig.InitImGui = TA_USE_IMGUI;
        rendererConfig.UseShaderCache = false;

        Input::Init(m_Window);

        auto& renderer = Application::Get().InitRenderer(rendererConfig);
        m_Renderer = &renderer;

        m_Camera2D = renderer.GetCamera2D();

        PXL_CREATE_AND_REGISTER_HANDLER(m_KeyDownHandler, KeyDownEvent, OnKeyDownEvent);
        // PXL_CREATE_AND_REGISTER_HANDLER(m_KeyUpHandler, KeyUpEvent, OnKeyUpEvent);
        PXL_CREATE_AND_REGISTER_HANDLER(m_MouseMoveHandler, MouseMoveEvent, OnMouseMoveEvent);

        // clang-format off
        m_ColourQuad = {
            .Position = Vec3(0),
            .Rotation = 0,
            .Size     = Vec2(350, 350),
            .Colour   = Colour::RGBA(86, 62, 223, 255),
            .Origin   = Origin2D::Centre,
            .Anchor   = Anchor2D::Centre,
        };

        m_TextureQuad = {
            .Position = Vec3(0, 0, 0),
            .Rotation = 0,
            .Size     = Vec2(32, 32) * Vec2(8),
            .Colour   = Colour::White(),
            .Origin   = Origin2D::Centre,
            .Anchor   = Anchor2D::Centre,
            .Scaling  = Scaling2D::ScaleDownToFit,
        };
        // clang-format on

        m_Quads.push_back(&m_ColourQuad);
        m_Quads.push_back(&m_TextureQuad);

        TextureSpecs texSpecs;
        texSpecs.Filter = SampleFilter::Nearest;

        texSpecs.Image = FileSystem::LoadImageFile("assets/textures/stone.png");
        m_StoneTexture = renderer.CreateTexture(texSpecs);

        // Load texture sheet
        texSpecs.Image = FileSystem::LoadImageFile("assets/textures/goblin-spritesheet.png");
        m_SheetTexture = renderer.CreateTexture(texSpecs);

        TextureSheet textureSheet(m_SheetTexture, 16, 1);
        m_TestSubTexture = textureSheet.GetSubTexture(4, 1);
        m_AnimatedTexture = { textureSheet.GetSubTexturesRange({ 1, 1 }, { 16, 1 }), 10 };
    }

    void QuadsTest::OnUpdate(float dt)
    {
        if (Input::IsKeyPressed(KeyCode::Escape))
        {
            Application::Get().Close();
            return;
        }

        m_TextureQuad.Rotation = pxl::Utils::RotationWrap(m_TextureQuad.Rotation + m_RotationSpeed * dt);
        m_ColourQuad.Rotation = pxl::Utils::RotationWrap(m_ColourQuad.Rotation - m_RotationSpeed * dt);
    }

    void QuadsTest::OnRender(Renderer& renderer)
    {
        for (uint32_t i = 0; i < m_QuadCount; i++)
        {
            Quad q = m_ColourQuad;
            q.Rotation += m_RotationOffset * i;
            renderer.Submit(q);
        }

        renderer.Submit(m_TextureQuad, m_AnimatedTexture);
    }

    void QuadsTest::OnGUIRender()
    {
#ifdef PXL_ENABLE_IMGUI
        ImGui::Begin("Quad Test");

        ImGui::SeparatorText("Quad Variables");
        const uint32_t minZero = 0;
        uint32_t maxQuad = m_Quads.size() - 1;
        ImGui::SliderScalar("Selected Quad", ImGuiDataType_U32, &m_QuadIndex, &minZero, &maxQuad);

        pxl::Quad* selectedQuad = m_Quads.at(m_QuadIndex);
        ImGui::DragFloat3("Position", glm::value_ptr(selectedQuad->Position));
        ImGui::DragFloat("Rotation", &selectedQuad->Rotation);
        ImGui::DragFloat2("Size", glm::value_ptr(selectedQuad->Size));
        ImGui::ColorEdit4("Colour", glm::value_ptr(selectedQuad->Colour));

        ImGui::SeparatorText("Other");

        const uint32_t stepOne = 1;
        ImGui::InputScalar("Rotation Index", ImGuiDataType_U32, &m_RotationOffset, &stepOne);
        ImGui::InputScalar("Rotation Speed", ImGuiDataType_Float, &m_RotationSpeed);
        ImGui::InputScalar("Quad Count", ImGuiDataType_U32, &m_QuadCount, &stepOne);

        const uint32_t minOne = 1;
        uint32_t maxFrame = m_AnimatedTexture.Frames.size() - 1;
        const float stepFloat = 0.1f;
        ImGui::SliderScalar("Sprite Frame", ImGuiDataType_U32, &m_AnimatedTexture.FrameIndex, &minZero, &maxFrame);
        ImGui::InputScalar("Sprite Framerate", ImGuiDataType_U32, &m_AnimatedTexture.Framerate, &stepOne);
        ImGui::InputScalar("Sprite Speed Multiplier", ImGuiDataType_Float, &m_AnimatedTexture.SpeedMultiplier, &stepFloat);

        ImGui::End();
#endif
    }

    void QuadsTest::OnKeyDownEvent(const KeyDownEvent& e)
    {
        if (e.IsKey(KeyCode::F7))
            m_Renderer->SetVerticalSync(!m_Renderer->GetConfig().VerticalSync);

        if (e.IsModsAndKey(KeyModFlags::Alt, KeyCode::Enter))
            m_Window->NextWindowMode();
    }

    void QuadsTest::OnMouseMoveEvent(const MouseMoveEvent& e)
    {
        m_CursorPos = e.GetPosition();
    }
}