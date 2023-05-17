#pragma once

#include "VertexBuffer.h"

namespace pxl
{
    class VertexArray
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetLayout(BufferLayout& layout) = 0;
    };
}