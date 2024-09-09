#pragma once

#include <glad/glad.h>

#include "Renderer/GPUBuffer.h"

namespace pxl
{
    class OpenGLBuffer : public GPUBuffer
    {
    public:
        OpenGLBuffer(GPUBufferUsage usage, uint32_t size, const void* data);
        OpenGLBuffer(GPUBufferUsage usage, uint32_t size);
        virtual ~OpenGLBuffer() override;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetData(uint32_t size, const void* data) override;
    private:
        static GLenum GetGLUsageEnumOfBufferUsage(GPUBufferUsage usage);
    private:
        uint32_t m_RendererID = 0;
        GLenum m_Usage;
    };
}