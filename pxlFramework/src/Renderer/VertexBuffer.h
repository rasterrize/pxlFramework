#pragma once

namespace pxl
{
    class VertexBuffer
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetData(int size, const void* data) = 0;
    };
}