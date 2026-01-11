#pragma once

#include <pxl/pxl.h>

#include "Test.h"

namespace TestApp
{
    class OGLVK : public Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs) override;
        virtual void OnUpdate(float dt) override;
        virtual void OnRender() override;
        virtual void OnGUIRender() override;

        virtual std::shared_ptr<pxl::Window> GetWindow() const { return m_Window; }

        virtual std::string ToString() const { return "OGLVK"; }

    private:
        std::shared_ptr<pxl::Window> m_Window = nullptr;
        std::shared_ptr<pxl::PerspectiveCamera> m_Camera = nullptr;
    };
}