#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

#include "../Renderer.h"

namespace pxl
{
    // Static Buffer
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, const void* data)
    {
        glCreateBuffers(1, &m_RendererID); // Create Buffer - NOTE: glCreateBuffers is supposed to bind the buffer at the same time, but from testing that doesnt seem to work
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // Write the data into the buffer
    }

    // Dynamic Buffer
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLVertexBuffer::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLVertexBuffer::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::SetData(uint32_t size, const void* data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
}