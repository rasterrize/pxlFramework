#pragma once

#include "GUIBase.h"

namespace pxl
{
    class GUIOpenGL : public GUIBase
    {
    public:
        GUIOpenGL();

        virtual void NewFrame() override;
        virtual void Render() override;
        virtual void Shutdown() override;
    };
}