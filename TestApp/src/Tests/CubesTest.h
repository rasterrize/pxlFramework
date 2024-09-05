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
        static void OnGUIRender();
    };
}