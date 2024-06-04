#pragma once

#include "../VertexArray.h"

#include <glad/glad.h>

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
    private:
        static GLenum GetOpenGLTypeOfBufferDataType(BufferDataType type);
    private:
        uint32_t m_RendererID;
    };
}