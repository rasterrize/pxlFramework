#pragma once

#include "../IndexBuffer.h"

namespace pxl
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t count, const void* data);
        OpenGLIndexBuffer(uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual const uint32_t GetCount() const { return m_Count; }

        virtual void SetData(uint32_t count, const void* data) override;
    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}