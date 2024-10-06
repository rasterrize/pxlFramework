#include "ModelViewer.h"

namespace TestApp
{
    static std::shared_ptr<pxl::Window> s_Window = nullptr;
    static std::shared_ptr<pxl::PerspectiveCamera> s_Camera = nullptr;

    static glm::vec4 m_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

    static glm::vec2 s_MouseDelta = glm::vec2(0.0f);

    static glm::vec3 s_MeshRotation = { 0.0f, 0.0f, 0.0f };
    static glm::vec3 s_MeshPosition = { 0.0f, 0.0f, 0.0f };

    static std::vector<std::vector<std::shared_ptr<pxl::Mesh>>> s_LoadedModels;
    static std::vector<std::string> s_LoadedModelNames;
    static int32_t s_CurrentModelIndex = 0;

    void ModelViewer::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        PXL_PROFILE_SCOPE;

        windowSpecs.Title += " - Running Test 'ModelViewer'";

        s_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(s_Window);
        pxl::Input::Init(s_Window);

        pxl::Renderer::SetClearColour(m_ClearColour);

        s_Camera = pxl::PerspectiveCamera::Create({ .FOV = 60.0f,
            .AspectRatio = 16.0f / 9.0f,
            .NearClip = 0.001f,
            .FarClip = 1000.0f });
        s_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::Quad, s_Camera);

        s_LoadedModels.push_back(pxl::FileSystem::LoadModel("assets/models/star.obj"));
        AddModelToList("assets/models/star.obj");

        s_Window->SetFileDropCallback(OnFileDrop);
        s_Window->SetResizeCallback([](pxl::Size2D newSize)
        {
            s_Camera->SetAspectRatio(static_cast<float>(newSize.Width) / static_cast<float>(newSize.Height));
        });

        pxl::GUI::Init(s_Window);
    }

    void ModelViewer::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        auto cameraPosition = s_Camera->GetPosition();
        auto cameraRotation = s_Camera->GetRotation();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ESCAPE))
        {
            pxl::Application::Get().Close();
            return;
        }

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_LEFT_ALT) && pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_ENTER))
            s_Window->NextWindowMode();

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_F7))
            s_Window->GetGraphicsContext()->SetVSync(!s_Window->GetGraphicsContext()->GetVSync());

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_W))
            cameraPosition.z -= 20.0f * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::PXL_KEY_S))
            cameraPosition.z += 20.0f * dt;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_DOWN))
            s_MeshPosition.y -= 30.0f * dt;

        if (pxl::Input::IsKeyPressed(pxl::KeyCode::PXL_KEY_UP))
            s_MeshPosition.y += 30.0f * dt;

        if (pxl::Input::IsMouseButtonHeld(pxl::MouseCode::PXL_MOUSE_BUTTON_LEFT))
        {
            s_MeshRotation.y += pxl::Input::GetCursorDelta().x;
            s_MeshRotation.x += pxl::Input::GetCursorDelta().y;
        }

        s_MeshRotation.x = std::fmodf(s_MeshRotation.x, 360.0f);
        s_MeshRotation.y = std::fmodf(s_MeshRotation.y, 360.0f);
        s_MeshRotation.z = std::fmodf(s_MeshRotation.z, 360.0f);

        s_Camera->SetRotation(cameraRotation);
        s_Camera->SetPosition(cameraPosition);
    }

    void ModelViewer::OnRender()
    {
        PXL_PROFILE_SCOPE;

        for (const auto& mesh : s_LoadedModels[s_CurrentModelIndex])
        {
            pxl::Renderer::DrawMesh(mesh, s_MeshPosition, s_MeshRotation, glm::vec3(1.0f));
        }
    }

    void ModelViewer::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        ImGui::Begin("ModelViewer");

        ImGui::Text("s_MeshRotation: %.2f, %.2f, %.2f", s_MeshRotation.x, s_MeshRotation.y, s_MeshRotation.z);
        ImGui::Text("CursorDelta: %.2f, %.2f", pxl::Input::GetCursorDelta().x, pxl::Input::GetCursorDelta().y);

        if (ImGui::BeginListBox("Models"))
        {
            for (int n = 0; n < s_LoadedModelNames.size(); n++)
            {
                const bool is_selected = (s_CurrentModelIndex == n);
                if (ImGui::Selectable(s_LoadedModelNames[n].c_str(), is_selected))
                    s_CurrentModelIndex = n;
            }

            ImGui::EndListBox();
        }

        ImGui::End();
    }

    void ModelViewer::OnClose()
    {
    }

    void ModelViewer::OnFileDrop(const std::vector<std::string>& paths)
    {
        // TODO: Check if file is a model file (.obj/.fbx/.gltf)
        s_LoadedModels.push_back(pxl::FileSystem::LoadModel(paths.at(0))); // NOTE: currently only uses the first path
        AddModelToList(paths.at(0));
        s_MeshRotation = glm::vec3(0.0f);
    }

    void ModelViewer::AddModelToList(const std::string& modelName)
    {
        size_t pos = modelName.rfind("/");

        if (pos >= modelName.length())
            pos = modelName.rfind("\\");

        s_LoadedModelNames.push_back(modelName.substr(pos + 1, modelName.length()));
    }
}