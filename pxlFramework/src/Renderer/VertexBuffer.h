#pragma once

#include "BufferLayout.h"

namespace pxl
{
    class VertexBuffer
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    };
}