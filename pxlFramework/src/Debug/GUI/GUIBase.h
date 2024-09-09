#pragma once

namespace pxl
{
    class GUIBase
    {
    public:
        virtual ~GUIBase() = default;

        virtual void NewFrame() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
    };
}