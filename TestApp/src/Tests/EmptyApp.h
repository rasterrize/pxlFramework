#pragma once

#include "Test.h"

namespace TestApp
{
    class EmptyApp : public Test
    {
    public:
        virtual void OnUpdate(float dt) override;

        virtual std::string ToString() const override { return "EmptyApp"; }

    private:
        uint32_t m_UpdateCount;

        const uint32_t m_MaxUpdateCount = 5;
    };
}