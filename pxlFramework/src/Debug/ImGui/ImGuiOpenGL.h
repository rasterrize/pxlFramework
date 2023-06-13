#pragma once

#include "ImGuiBase.h"

namespace pxl
{
    class ImGuiOpenGL : public ImGuiBase
    {
    public:
        ImGuiOpenGL();

        virtual void NewFrame() override;
        virtual void Render() override;
        virtual void Shutdown() override;
    };
}