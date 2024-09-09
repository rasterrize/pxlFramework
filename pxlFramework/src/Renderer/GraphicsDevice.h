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

        virtual void* GetLogical() const = 0;
        virtual void* GetPhysical() const = 0;

        virtual void WaitIdle() const = 0;

        virtual const DeviceLimits& GetDeviceLimits() const = 0;
    };
}