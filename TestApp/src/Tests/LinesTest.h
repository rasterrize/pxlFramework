#pragma once

#include <pxl/pxl.h>

#include "Test.h"

namespace TestApp
{
    class LinesTest : public Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs) override;
        virtual void OnUpdate(float dt) override;
        virtual void OnRender(const std::unique_ptr<pxl::Renderer>& renderer) override;

        virtual std::shared_ptr<pxl::Window> GetWindow() const override { return m_Window; }

        virtual std::string ToString() const override { return "LinesTest"; }

    private:
        std::shared_ptr<pxl::Window> m_Window = nullptr;
        std::shared_ptr<pxl::PerspectiveCamera> m_Camera = nullptr;
    };
}