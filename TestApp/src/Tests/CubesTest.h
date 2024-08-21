#pragma once

#include <pxl/pxl.h>

namespace TestApp
{
    class CubesTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnClose();
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnGuiRender();
    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::PerspectiveCamera> m_Camera;
        static glm::vec4 m_ClearColour;

        static glm::vec3 m_PlayerPosition;

        static bool controllingCamera;
    };
}