#pragma once

namespace pxl
{
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetData(uint32_t size, const void* data) = 0;
    };
}