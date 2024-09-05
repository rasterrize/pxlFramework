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
        static void OnGUIRender();
        static void OnClose();
    };
}