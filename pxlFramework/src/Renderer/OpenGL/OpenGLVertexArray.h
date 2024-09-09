#pragma once

#include <glad/glad.h>

#include "Renderer/VertexArray.h"

namespace pxl
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void AddVertexBuffer(const std::shared_ptr<GPUBuffer>& vertexBuffer, const BufferLayout& layout) override;
        virtual void SetIndexBuffer(const std::shared_ptr<GPUBuffer>& indexBuffer) override;
    private:
        static GLenum GetOpenGLTypeOfBufferDataType(BufferDataType type);
    private:
        uint32_t m_RendererID = 0;
    };
}