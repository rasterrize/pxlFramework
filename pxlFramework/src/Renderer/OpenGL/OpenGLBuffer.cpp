#include "OpenGLBuffer.h"

namespace pxl
{
    OpenGLBuffer::OpenGLBuffer(BufferUsage usage, uint32_t size, const void* data)
    {
        m_Usage = GetGLUsageEnumOfBufferUsage(usage);
        glCreateBuffers(1, &m_RendererID); // Create Buffer - NOTE: glCreateBuffers is supposed to bind the buffer at the same time, but from testing that doesnt seem to work
        glBindBuffer(m_Usage, m_RendererID);
        glBufferData(m_Usage, size, data, GL_STATIC_DRAW); // Write the data into the buffer
    }

    OpenGLBuffer::OpenGLBuffer(BufferUsage usage, uint32_t size)
    {
        m_Usage = GetGLUsageEnumOfBufferUsage(usage);
        glCreateBuffers(1, &m_RendererID); // Create Buffer - NOTE: glCreateBuffers is supposed to bind the buffer at the same time, but from testing that doesnt seem to work
        glBindBuffer(m_Usage, m_RendererID);
        glBufferData(m_Usage, size, nullptr, GL_DYNAMIC_DRAW); // Write the data into the buffer
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

    GLenum OpenGLBuffer::GetGLUsageEnumOfBufferUsage(BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::None:
                PXL_LOG_ERROR(LogArea::OpenGL, "Buffer usage was none, can't convert to GLBufferUsage");
                break;
            case BufferUsage::Vertex:
                return GL_ARRAY_BUFFER;
            case BufferUsage::Index:
                return GL_ELEMENT_ARRAY_BUFFER;
        }

        return GL_INVALID_ENUM;
    }
}