#pragma once

#include "../VertexBuffer.h"
#include "../BufferLayout.h"

namespace pxl
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(int size, const void* data);

        virtual void Bind() override;
        virtual void Unbind() override;
    private:
        unsigned int m_RendererID;
    };
}