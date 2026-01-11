#pragma once

#include <pxl/pxl.h>

#include "Test.h"

namespace TestApp
{
    class MultiWindow : public Test
    {
    public:
        virtual void OnStart(pxl::WindowSpecs& windowSpecs);
        virtual void OnUpdate(float dt);

        virtual std::shared_ptr<pxl::Window> GetWindow() const { return nullptr; }

        virtual std::string ToString() const override { return "MultiWindow"; }

    private:
        std::shared_ptr<pxl::Window> m_TestWindowA = nullptr;
        std::shared_ptr<pxl::Window> m_TestWindowB = nullptr;
    };
}