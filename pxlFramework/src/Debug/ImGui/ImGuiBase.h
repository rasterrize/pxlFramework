#pragma once

namespace pxl
{
    class ImGuiBase
    {
    public:
        virtual void NewFrame() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
    };
}