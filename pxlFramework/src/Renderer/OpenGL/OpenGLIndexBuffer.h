#pragma once

#include "../IndexBuffer.h"

namespace pxl
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(unsigned int count, unsigned int* data);
        OpenGLIndexBuffer();

        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual const unsigned int GetCount() const { return m_Count; }

        virtual void SetData(int count, const void* data) override;
    private:
        unsigned int m_RendererID;
        unsigned int m_Count;
    };
}