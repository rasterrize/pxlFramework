#pragma once

namespace pxl
{
    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void Destroy() = 0;
    };
}