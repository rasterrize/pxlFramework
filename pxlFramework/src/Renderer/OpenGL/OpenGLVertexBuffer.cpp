#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

#include "../Renderer.h"

namespace pxl
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(int size, const void* data)
    {
        glCreateBuffers(1, &m_RendererID); // Create Buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); // Access this buffer (OpenGL is a state machine)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // Write the data into the buffer
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);
    }

    void OpenGLVertexBuffer::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLVertexBuffer::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::SetData(int size, const void *data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
}