#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace pxl
{
    class VertexArray
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetLayout(BufferLayout& layout) = 0;

        virtual void SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) = 0; 
        virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) = 0;

        virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() = 0;
        virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() = 0;
    };
}