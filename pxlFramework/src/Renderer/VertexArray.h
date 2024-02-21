#pragma once

#include "BufferLayout.h"
#include "Buffer.h"

namespace pxl
{
    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void AddVertexBuffer(const std::shared_ptr<Buffer>& vertexBuffer, const BufferLayout& layout) = 0; 
        virtual void SetIndexBuffer(const std::shared_ptr<Buffer>& indexBuffer) = 0;

        virtual std::shared_ptr<Buffer> GetVertexBuffer() = 0;
        virtual std::shared_ptr<Buffer> GetIndexBuffer() = 0;

        static std::shared_ptr<VertexArray> Create();
    };
}