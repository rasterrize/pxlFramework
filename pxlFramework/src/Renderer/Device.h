#pragma once

namespace pxl
{
    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void* GetDevice() = 0;
        virtual void* GetPhysicalDevice() = 0;

        virtual void WaitIdle() = 0;
    };
}