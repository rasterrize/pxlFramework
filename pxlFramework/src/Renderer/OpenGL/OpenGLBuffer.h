#pragma once

#include <glad/glad.h>

#include "Renderer/GPUBuffer.h"

namespace pxl
{
    class OpenGLBuffer : public GPUBuffer
    {
    public:
        OpenGLBuffer(GPUBufferUsage usage, GPUBufferDrawHint drawHint, uint32_t size, const void* data);
        virtual ~OpenGLBuffer() override;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetData(uint32_t size, const void* data) override;

    private:
        static GLenum ToGLUsageEnum(GPUBufferUsage usage);
        static GLenum ToGLDrawHint(GPUBufferDrawHint usage);

    private:
        uint32_t m_RendererID = 0;
        GLenum m_Usage = GL_INVALID_ENUM;
        GLenum m_DrawHint = GL_INVALID_ENUM;
    };
}