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
    private:
        unsigned int m_RendererID;
    };
}