#include "OpenGLBuffer.h"

namespace pxl
{
    OpenGLBuffer::OpenGLBuffer(GPUBufferUsage usage, uint32_t size, const void* data)
        : m_Usage(GetGLUsageEnumOfBufferUsage(usage))
    {
        glCreateBuffers(1, &m_RendererID);                 // "CreateBuffers" instead of "GenBuffers" initializes the object on creation rather than on binding. However, The buffer still needs to be bound to the current OGL context
        glBindBuffer(m_Usage, m_RendererID);               // Initializes the buffer object (not the data) to the context
        glBufferData(m_Usage, size, data, GL_STATIC_DRAW); // Initialize the data storage and copy the data into the memory

#ifdef PXL_ENABLE_LOGGING
        switch (usage)
        {
            case GPUBufferUsage::None:
                PXL_LOG_INFO(LogArea::OpenGL, "Created OpenGL buffer with no specified usage containing {} bytes of data", size);
                break;

            case GPUBufferUsage::Vertex:
                PXL_LOG_INFO(LogArea::OpenGL, "Created OpenGL vertex buffer containing {} bytes of data", size);
                break;

            case GPUBufferUsage::Index:
                PXL_LOG_INFO(LogArea::OpenGL, "Created OpenGL index buffer containing {} bytes of data", size);
                break;
        }
#endif
    }

    OpenGLBuffer::OpenGLBuffer(GPUBufferUsage usage, uint32_t size)
        : m_Usage(GetGLUsageEnumOfBufferUsage(usage))
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(m_Usage, m_RendererID);
        glBufferData(m_Usage, size, nullptr, GL_DYNAMIC_DRAW); // Initializes the data storage but doesn't copy any data into its memory. The memory therefore is undefined

#ifdef PXL_ENABLE_LOGGING
        switch (usage)
        {
            case GPUBufferUsage::None:
                PXL_LOG_INFO(LogArea::OpenGL, "Created OpenGL buffer with no specified usage containing no data", size);
                break;

            case GPUBufferUsage::Vertex:
                PXL_LOG_INFO(LogArea::OpenGL, "Created OpenGL vertex buffer containing no data", size);
                break;

            case GPUBufferUsage::Index:
                PXL_LOG_INFO(LogArea::OpenGL, "Created OpenGL index buffer containing no data", size);
                break;
        }
#endif
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

    GLenum OpenGLBuffer::GetGLUsageEnumOfBufferUsage(GPUBufferUsage usage)
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
}