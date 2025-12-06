#pragma once

#include <pxl/pxl.h>

namespace TestApp
{
    class AudioTest
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnClose();
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnGUIRender();
        static std::shared_ptr<pxl::Window> GetWindow();
    };
}