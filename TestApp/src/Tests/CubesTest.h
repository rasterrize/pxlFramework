#pragma once

#include <pxl/pxl.h>

#include "Test.h"

namespace TestApp
{
    class CubesTest : public Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs) override;
        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGUIRender() override;

        virtual std::shared_ptr<pxl::Window> GetWindow() const override { return m_Window; }

        virtual std::string ToString() const override { return "CubesTest"; }

    private:
        std::shared_ptr<pxl::Window> m_Window = nullptr;
        std::shared_ptr<pxl::PerspectiveCamera> m_Camera = nullptr;
    };
}