#pragma once

#include "BufferLayout.h"
#include "GPUBuffer.h"

namespace pxl
{
    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void AddVertexBuffer(const std::shared_ptr<GPUBuffer>& vertexBuffer, const BufferLayout& layout) = 0;
        virtual void SetIndexBuffer(const std::shared_ptr<GPUBuffer>& indexBuffer) = 0;

        static std::shared_ptr<VertexArray> Create();
    };
}