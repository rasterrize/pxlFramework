#include "OpenGLVertexBuffer.h"
#include "glad/glad.h"

namespace pxl
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(int size, const void* data)
    {
        glCreateBuffers(1, &m_RendererID); // Create Buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); // Access this buffer (OpenGL is a state machine)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // Write the data into the buffer
    }

    void OpenGLVertexBuffer::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLVertexBuffer::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}