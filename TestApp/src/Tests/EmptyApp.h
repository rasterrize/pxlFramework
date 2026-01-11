#pragma once

#include "Test.h"

namespace TestApp
{
    class EmptyApp : public Test
    {
    public:
        virtual void OnUpdate(float dt) override;

        virtual std::string ToString() const override { return "EmptyApp"; }
    };
}