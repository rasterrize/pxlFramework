#pragma once

#include "../VertexArray.h"

#include <glad/glad.h>

#include "../Buffer.h"
#include "../BufferLayout.h"

namespace pxl
{
    class  OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void AddVertexBuffer(const std::shared_ptr<Buffer>& vertexBuffer, const BufferLayout& layout) override;
        virtual void SetIndexBuffer(const std::shared_ptr<Buffer>& indexBuffer) override;

        virtual std::shared_ptr<Buffer> GetVertexBuffer() override { return nullptr; } // TODO
        virtual std::shared_ptr<Buffer> GetIndexBuffer() override { return m_IndexBuffer; }
    private:
        static GLenum GetOpenGLTypeOfBufferDataType(BufferDataType type);
    private:
        uint32_t m_RendererID;
        
        std::vector<std::shared_ptr<Buffer>> m_VertexBuffers; // Vertex arrays can control more than 1 vertex buffer
        std::shared_ptr<Buffer> m_IndexBuffer;
    };
}