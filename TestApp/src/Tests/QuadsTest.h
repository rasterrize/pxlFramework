#pragma once

#include <pxl/pxl.h>

namespace TestApp
{
    class QuadsTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnGuiRender();
        static void OnClose();
    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::OrthographicCamera> m_Camera;
        static glm::vec4 m_ClearColour;

        static bool m_ControllingCamera;

        static glm::vec4 m_QuadColour;

        static glm::vec2 m_SelectedTile;

        static uint32_t m_BlueQuadAmount;
        static uint32_t m_OrangeQuadAmount;

        static std::shared_ptr<pxl::Texture> m_StoneTexture;
    };
}