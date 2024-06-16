#pragma once

#include <pxl.h>

namespace TestApp
{
    class QuadsTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnImGuiRender();
        static void OnClose();
    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::Camera> m_Camera;
        static std::shared_ptr<pxl::Shader> m_Shader; 
        static std::shared_ptr<pxl::GraphicsPipeline> m_Pipeline;
        static glm::vec4 m_ClearColour;

        static glm::vec3 m_CameraPosition;
        static glm::vec3 m_CameraRotation;

        static bool m_ControllingCamera;

        static glm::vec4 m_QuadColour;

        static glm::vec2 m_SelectedTile;

        static uint32_t m_BlueQuadAmount;
        static uint32_t m_OrangeQuadAmount;

        static std::shared_ptr<pxl::Texture2D> m_StoneTexture;
    };
}