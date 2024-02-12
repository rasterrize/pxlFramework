#pragma once

#include "../VertexArray.h"

#include "../Buffer.h"
#include "../BufferLayout.h"

namespace pxl
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetLayout(const BufferLayout& layout) override;

        virtual void SetVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override { m_VertexBuffer.swap(vertexBuffer); }
        virtual void SetIndexBuffer(std::shared_ptr<Buffer> indexBuffer) override { m_IndexBuffer.swap(indexBuffer); }

        virtual std::shared_ptr<Buffer> GetVertexBuffer() override { return m_VertexBuffer; }
        virtual std::shared_ptr<Buffer> GetIndexBuffer() override { return m_IndexBuffer; }
    private:
        static GLenum GetOpenGLTypeOfBufferDataType(BufferDataType type);
    private:
        uint32_t m_RendererID;
        
        std::shared_ptr<Buffer> m_VertexBuffer;
        std::shared_ptr<Buffer> m_IndexBuffer;
    };
}