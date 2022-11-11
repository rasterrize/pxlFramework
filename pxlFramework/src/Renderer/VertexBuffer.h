#pragma once

#include "BufferLayout.h"

namespace pxl
{
    class VertexBuffer
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual const BufferLayout GetLayout() = 0;

        virtual void AddLayout(const BufferLayout& layout) = 0;
    };
}