#pragma once

#include "../VertexBuffer.h"
#include "../BufferLayout.h"

namespace pxl
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(uint32_t size, const void* data);
        OpenGLVertexBuffer(uint32_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetData(uint32_t size, const void* data) override;
    private:
        uint32_t m_RendererID;
    };
}