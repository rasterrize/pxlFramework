#pragma once

#include <pxl.h>

namespace TestApp
{
    class WindowTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnImGuiRender();
        static void OnClose();
    private:
        static std::shared_ptr<pxl::Window> m_TestWindowA;
        static std::shared_ptr<pxl::Window> m_TestWindowB;
    };
}