#include "OpenGLBuffer.h"

namespace pxl
{
    OpenGLBuffer::OpenGLBuffer(GPUBufferUsage usage, GPUBufferDrawHint drawHint, uint32_t size, const void* data)
        : m_Usage(ToGLUsageEnum(usage)), m_DrawHint(ToGLDrawHint(drawHint))
    {
        glCreateBuffers(1, &m_RendererID);             // "CreateBuffers" instead of "GenBuffers" initializes the object on creation rather than on binding. However, The buffer still needs to be bound to the current OGL context
        glBindBuffer(m_Usage, m_RendererID);           // Initializes the buffer object (not the data) to the context
        glBufferData(m_Usage, size, data, m_DrawHint); // Initialize the data storage and copy the data into the memory
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLBuffer::Bind()
    {
        glBindBuffer(m_Usage, m_RendererID);
    }

    void OpenGLBuffer::Unbind()
    {
        glBindBuffer(m_Usage, 0);
    }

    void OpenGLBuffer::SetData(uint32_t size, const void* data)
    {
        glBindBuffer(m_Usage, m_RendererID);
        glBufferSubData(m_Usage, 0, size, data);
    }

    GLenum OpenGLBuffer::ToGLUsageEnum(GPUBufferUsage usage)
    {
        switch (usage)
        {
            case GPUBufferUsage::None:
                PXL_LOG_ERROR(LogArea::OpenGL, "Buffer usage was none, can't convert to GLBufferUsage");
                return GL_INVALID_ENUM;

            case GPUBufferUsage::Vertex: return GL_ARRAY_BUFFER;
            case GPUBufferUsage::Index:  return GL_ELEMENT_ARRAY_BUFFER;
        }

        return GL_INVALID_ENUM;
    }

    GLenum OpenGLBuffer::ToGLDrawHint(GPUBufferDrawHint usage)
    {
        switch (usage)
        {
            case GPUBufferDrawHint::Static:  return GL_STATIC_DRAW;
            case GPUBufferDrawHint::Dynamic: return GL_DYNAMIC_DRAW;
        }

        return GL_INVALID_ENUM;
    }
}