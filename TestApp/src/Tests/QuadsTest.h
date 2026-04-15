#pragma once

#include <pxl/pxl.h>

#include "Test.h"

namespace TestApp
{
    class QuadsTest : public Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs) override;
        virtual void OnUpdate(float dt) override;
        virtual void OnRender(pxl::Renderer& renderer) override;
        virtual void OnGUIRender() override;

        virtual std::shared_ptr<pxl::Window> GetWindow() const override { return m_Window; }

        virtual std::string ToString() const override { return "QuadsTest"; }

        void OnKeyDownEvent(const pxl::KeyDownEvent& e);
        void OnMouseMoveEvent(const pxl::MouseMoveEvent& e);

    private:
        std::shared_ptr<pxl::Window> m_Window = nullptr;
        std::shared_ptr<pxl::OrthographicCamera> m_Camera = nullptr;

        pxl::Renderer* m_Renderer;
        std::shared_ptr<pxl::Camera> m_Camera2D;

        pxl::UserEventHandler<pxl::KeyDownEvent> m_KeyDownHandler;
        pxl::UserEventHandler<pxl::KeyUpEvent> m_KeyUpHandler;
        pxl::UserEventHandler<pxl::MouseMoveEvent> m_MouseMoveHandler;

        std::shared_ptr<pxl::Texture> m_StoneTexture = nullptr;
        std::shared_ptr<pxl::Texture> m_CursorTexture = nullptr;
        std::shared_ptr<pxl::Texture> m_SheetTexture = nullptr;

        pxl::AnimatedTexture m_AnimatedTexture;

        pxl::SubTexture m_TestSubTexture;

        std::vector<pxl::Quad*> m_Quads;
        uint32_t m_QuadIndex = 0;
        pxl::Quad m_ColourQuad;
        pxl::Quad m_TextureQuad;

        uint32_t m_RotationOffset = 0;
        uint32_t m_QuadCount = 1;
        float m_RotationSpeed = 10.0f;

        glm::dvec2 m_CursorPos;

        pxl::RectF m_QuadBoundingBox;
    };
}