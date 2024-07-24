#pragma once

#include <pxl.h>

namespace TestApp
{
    class LinesTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnGuiRender();
        static void OnClose();
    private:
        static std::shared_ptr<pxl::Window> m_Window;
        static std::shared_ptr<pxl::Camera> m_Camera;
    };
}