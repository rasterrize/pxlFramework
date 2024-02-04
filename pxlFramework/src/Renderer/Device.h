#pragma once

namespace pxl
{
    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void* GetLogicalDevice() = 0;
        virtual void* GetPhysicalAdapter() = 0;

        virtual void Destroy() = 0;
    };
}