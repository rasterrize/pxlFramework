#pragma once

namespace pxl
{
    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void Destroy() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
    };
}