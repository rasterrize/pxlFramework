#pragma once

#include "../VertexArray.h"
#include "../VertexBuffer.h"

namespace pxl
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetLayout(BufferLayout& layout) override;

        virtual void SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) override { m_VertexBuffer.swap(vertexBuffer); }
        virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) override { m_IndexBuffer.swap(indexBuffer); }

        virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() override { return m_VertexBuffer; }
        virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() override { return m_IndexBuffer; }
    private:
        unsigned int m_RendererID;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}