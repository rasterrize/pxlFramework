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

        virtual void* GetDevice() const = 0;
        virtual void* GetPhysicalDevice() const = 0;

        virtual void WaitIdle() const = 0;

        virtual const DeviceLimits& GetDeviceLimits() const = 0;
    };
}