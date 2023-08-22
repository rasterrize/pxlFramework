#pragma once

namespace pxl
{
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual const uint32_t GetCount() const = 0;

        virtual void SetData(uint32_t count, const void* data) = 0;
    };
}