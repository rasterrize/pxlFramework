#pragma once

#include <pxl/pxl.h>

namespace TestApp
{
    class Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs) {}
        virtual void OnUpdate(float dt) {}
        virtual void OnRender() {}
        virtual void OnGUIRender() {}
        virtual void OnClose() {}

        virtual std::shared_ptr<pxl::Window> GetWindow() const { return nullptr; }

        virtual std::string ToString() const = 0;
    };
}