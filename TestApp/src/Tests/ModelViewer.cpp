#include "ModelViewer.h"

namespace TestApp
{
    void ModelViewer::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        PXL_PROFILE_SCOPE;

        m_Window = pxl::Window::Create(windowSpecs);

        pxl::Renderer::Init(m_Window);
        pxl::Input::Init(m_Window);

        pxl::Renderer::SetClearColour(m_ClearColour);

        m_Camera = pxl::Camera::CreatePerspective({ .FOV = 60.0f,
            .AspectRatio = m_Window->GetAspectRatio(),
            .NearClip = 0.001f,
            .FarClip = 1000.0f });

        m_Camera->SetPosition({ 0.0f, 0.0f, 5.0f });

        pxl::Renderer::SetCamera(pxl::RendererGeometryTarget::Quad, m_Camera);

        LoadMesh("assets/models/star.obj");

        pxl::GUI::Init(m_Window);

        PXL_REGISTER_HANDLER(m_ResizeHandler, pxl::WindowResizeEvent, OnWindowResizeEvent);
        PXL_REGISTER_HANDLER(m_PathDropHandler, pxl::WindowPathDropEvent, OnWindowPathDropEvent);
        PXL_REGISTER_HANDLER(m_KeyDownHandler, pxl::KeyDownEvent, OnKeyDownEvent);
    }

    void ModelViewer::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        auto cameraPosition = m_Camera->GetPosition();
        auto cameraRotation = m_Camera->GetRotation();

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::W))
            cameraPosition.z -= 20.0f * dt;

        if (pxl::Input::IsKeyHeld(pxl::KeyCode::S))
            cameraPosition.z += 20.0f * dt;

        if (pxl::Input::IsMouseButtonHeld(pxl::MouseCode::LeftButton) && !ImGui::GetIO().WantCaptureMouse)
        {
            m_MeshRotation.y += pxl::Input::GetCursorDelta().x;
            m_MeshRotation.x += pxl::Input::GetCursorDelta().y;
        }
        else
        {
            m_MeshRotation.y += 30.0f * dt;
        }

        m_MeshRotation.x = std::fmod(m_MeshRotation.x, 360.0f);
        m_MeshRotation.y = std::fmod(m_MeshRotation.y, 360.0f);
        m_MeshRotation.z = std::fmod(m_MeshRotation.z, 360.0f);

        m_Camera->SetRotation(cameraRotation);
        m_Camera->SetPosition(cameraPosition);
    }

    void ModelViewer::OnRender()
    {
        PXL_PROFILE_SCOPE;

        for (const auto& mesh : m_LoadedModels[m_CurrentModelIndex])
        {
            pxl::Renderer::DrawMesh(mesh, m_MeshPosition, m_MeshRotation, glm::vec3(1.0f));
        }
    }

    void ModelViewer::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;

        ImGui::Begin("ModelViewer");

        ImGui::DragFloat3("MeshRotation: %.2f, %.2f, %.2f", reinterpret_cast<float*>(&m_MeshRotation));
        ImGui::Text("CursorDelta: %.2f, %.2f", pxl::Input::GetCursorDelta().x, pxl::Input::GetCursorDelta().y);

        if (ImGui::BeginListBox("Models"))
        {
            for (int n = 0; n < m_LoadedModelNames.size(); n++)
            {
                const bool is_selected = (m_CurrentModelIndex == n);
                if (ImGui::Selectable(m_LoadedModelNames[n].c_str(), is_selected))
                    m_CurrentModelIndex = n;
            }

            ImGui::EndListBox();
        }

        if (ImGui::Button("Open File..."))
        {
            auto filePath = pxl::Platform::OpenFile(m_Window,
                "OBJ File (*.obj)\0"
                "*.obj\0");

            if (!filePath.empty())
                LoadMesh(filePath);
        }

        ImGui::End();
    }

    void ModelViewer::OnKeyDownEvent(pxl::KeyDownEvent& e)
    {
        if (e.IsKey(pxl::KeyCode::Escape))
        {
            pxl::Application::Get().Close();
            e.Handled();
            return;
        }

        if (e.IsModsAndKey(pxl::KeyModFlags::Alt, pxl::KeyCode::Enter))
            m_Window->NextWindowMode();

        if (e.IsKey(pxl::KeyCode::F7))
            m_Window->GetGraphicsContext()->SetVSync(!m_Window->GetGraphicsContext()->GetVSync());

        if (e.IsKey(pxl::KeyCode::F1))
        {
            auto filePath = pxl::Platform::OpenFile(m_Window,
                "OBJ File (*.obj)\0"
                "*.obj\0");

            if (!filePath.empty())
                LoadMesh(filePath);
        }

        if (e.IsKey(pxl::KeyCode::Down))
            m_MeshPosition.y -= 3.0f;

        if (e.IsKey(pxl::KeyCode::Up))
            m_MeshPosition.y += 3.0f;
    }

    void ModelViewer::OnWindowPathDropEvent(pxl::WindowPathDropEvent& e)
    {
        LoadMesh(e.GetPaths().at(0));
    }

    void ModelViewer::OnWindowResizeEvent(pxl::WindowResizeEvent& e)
    {
        auto newSize = e.GetSize();
        m_Camera->SetAspectRatio(static_cast<float>(newSize.Width) / static_cast<float>(newSize.Height));
    }

    void ModelViewer::LoadMesh(const std::filesystem::path& path)
    {
        auto extension = path.extension();

        std::vector<std::filesystem::path> supportedExtensions = {
            ".obj", ".fbx", ".gltf", ".glb"
        };

        bool validExtension = false;

        // SEXtension
        for (const auto& sExtension : supportedExtensions)
        {
            if (extension == sExtension)
                validExtension = true;
        }

        if (!validExtension)
        {
            APP_LOG_ERROR("Model file extension is invalid");
            return;
        }

        m_LoadedModels.push_back(pxl::FileSystem::LoadModel(path)); // NOTE: currently only uses the first path
        AddModelToList(path.string());
        m_MeshRotation = glm::vec3(0.0f);
        m_CurrentModelIndex = static_cast<int32_t>(m_LoadedModelNames.size() - 1);
    }

    void ModelViewer::AddModelToList(const std::string& modelName)
    {
        size_t pos = modelName.rfind("/");

        if (pos >= modelName.length())
            pos = modelName.rfind("\\");

        m_LoadedModelNames.push_back(modelName.substr(pos + 1, modelName.length()));
    }
}