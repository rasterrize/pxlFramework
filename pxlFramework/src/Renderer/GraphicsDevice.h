#pragma once

namespace pxl
{
    struct GraphicsDeviceLimits
    {
        uint32_t MaxTextureSlots = 32;
        float VRAMLimit = 128.0f; // VRAM limit in megabytes
        float MaxAnisotropicLevel = 1.0f;
        bool nonSolidFillModesSupport = false; // technically a feature, not a 'limit'
    };

    enum class QueueType
    {
        Graphics,
        Compute,
    };

    // Represents a Graphics Processing Device (GPU)
    class GraphicsDevice
    {
    public:
        virtual ~GraphicsDevice() = default;

        virtual void* GetLogical() const = 0;
        virtual void* GetPhysical() const = 0;

        virtual void WaitIdle() const = 0;
        virtual void QueueWaitIdle(QueueType queue) const = 0;

        virtual const GraphicsDeviceLimits& GetLimits() const = 0;
    };
}