#pragma once

namespace pxl
{
    struct DeviceLimits
    {
        // MaxTextureSlots
        // VRAM limit
        // ...
    };
    
    class GraphicsDevice
    {
    public:
        virtual ~GraphicsDevice() = default;

        virtual void* GetDevice() = 0;
        virtual void* GetPhysicalDevice() = 0;

        virtual void WaitIdle() = 0;

        virtual const DeviceLimits& GetDeviceLimits() = 0;
    };
}