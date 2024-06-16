#pragma once

#include <pxl.h>

namespace TestApp
{
    class EmptyApp
    {
    public:
        static void OnStart(pxl::WindowSpecs& windowSpecs);
        static void OnUpdate(float dt);
        static void OnRender();
        static void OnImGuiRender();
        static void OnClose();
    };
}