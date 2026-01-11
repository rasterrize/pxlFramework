#pragma once

#include <pxl/pxl.h>

#include "Test.h"

namespace TestApp
{
    class ModelViewer : public Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs) override;
        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGUIRender() override;

        virtual std::shared_ptr<pxl::Window> GetWindow() const override { return m_Window; }

        virtual std::string ToString() const override { return "ModelViewer"; }

        void OnWindowPathDropEvent(pxl::WindowPathDropEvent& e);
        void OnWindowResizeEvent(pxl::WindowResizeEvent& e);
        void OnKeyDownEvent(pxl::KeyDownEvent& e);

        void LoadMesh(const std::filesystem::path& path);
        void AddModelToList(const std::string& modelName);

    private:
        std::shared_ptr<pxl::Window> m_Window = nullptr;
        std::shared_ptr<pxl::PerspectiveCamera> m_Camera = nullptr;

        glm::vec4 m_ClearColour = { 0.078f, 0.094f, 0.109f, 1.0f };

        glm::vec2 m_MouseDelta = glm::vec2(0.0f);

        glm::vec3 m_MeshPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_MeshRotation = { 0.0f, 0.0f, 0.0f };

        std::vector<std::vector<std::shared_ptr<pxl::Mesh>>> m_LoadedModels;
        std::vector<std::string> m_LoadedModelNames;
        int32_t m_CurrentModelIndex = 0;

        pxl::UserEventHandler<pxl::WindowResizeEvent> m_ResizeHandler;
        pxl::UserEventHandler<pxl::WindowPathDropEvent> m_PathDropHandler;
        pxl::UserEventHandler<pxl::KeyDownEvent> m_KeyDownHandler;
    };
}