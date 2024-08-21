#include "ModelViewer.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_Window = nullptr;
    static std::shared_ptr<pxl::PerspectiveCamera> s_Camera = nullptr;

    static glm::vec4 m_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

    static std::shared_ptr<pxl::Mesh> s_CurrentMesh = nullptr;
    static std::shared_ptr<pxl::Mesh> s_AltMesh = nullptr;
    static glm::vec2 s_MouseDelta = glm::vec2(0.0f);
    static glm::vec3 s_MeshRotation = { 45.0f, 45.0f, 0.0f };

    void ModelViewer::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        PXL_PROFILE_SCOPE;
        
        windowSpecs.Title += " - Running Test 'ModelViewer'";

        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);

        pxl::Renderer::SetClearColour(m_ClearColour);

        s_Camera = pxl::PerspectiveCamera::Create({
            .FOV = 45.0f,
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = 0.01f,
            .FarClip = 1000.0f 
        });
        s_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetQuadsCamera(s_Camera);

        s_CurrentMesh = pxl::FileSystem::LoadOBJ("assets/models/golf_ball.obj");

        pxl::GUI::Init(s_Window);
    }

    void ModelViewer::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;
        
        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            s_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            s_Window->ToggleVSync();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_M))
            std::swap(s_CurrentMesh, s_AltMesh);

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
        {
            auto cameraPos = s_Camera->GetPosition();
            s_Camera->SetPosition({ cameraPos.x, cameraPos.y, cameraPos.z + 0.5f * dt });
        }

        s_MeshRotation.x += 10.0f * dt;
        s_MeshRotation.y += 10.0f * dt;
    }
        
    void ModelViewer::OnRender()
    {
        PXL_PROFILE_SCOPE;
        
        pxl::Renderer::Clear();

        pxl::Renderer::DrawMesh(s_CurrentMesh, glm::vec3(0.0f), s_MeshRotation, glm::vec3(1.0f));
    }

    void ModelViewer::OnGuiRender()
    {
        auto rendererStats = pxl::Renderer::GetStats();

        ImGui::Begin("Renderer Stats");

        ImGui::Text("FPS: %.2f", pxl::Renderer::GetFPS());
        ImGui::Text("Frame Time (MS): %.3f", pxl::Renderer::GetFrameTimeMS());
        ImGui::Text("Draw Calls: %u", rendererStats.DrawCalls);
        ImGui::Text("Total Triangle Count: %u", rendererStats.GetTotalTriangleCount());
        ImGui::Text("Total Vertex Count: %u", rendererStats.GetTotalVertexCount());
        ImGui::Text("Total Index Count: %u", rendererStats.GetTotalIndexCount());

        ImGui::End();
    }

    void ModelViewer::OnClose()
    {

    }
}