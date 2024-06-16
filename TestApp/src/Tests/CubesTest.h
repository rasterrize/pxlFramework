#pragma once

#include <pxl.h>

namespace TestApp
{
    class CubesTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnClose();
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnImGuiRender();

    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::Camera> m_Camera;
        static std::shared_ptr<pxl::Shader> m_Shader; 
        static std::shared_ptr<pxl::GraphicsPipeline> m_Pipeline;
        static glm::vec4 m_ClearColour;

        static glm::vec3 m_CameraPosition;
        static glm::vec3 m_CameraRotation;

        static glm::vec3 m_PlayerPosition;

        static bool controllingCamera;
    };
}