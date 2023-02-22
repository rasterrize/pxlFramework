#include "OpenGLIndexBuffer.h"
#include <glad/glad.h>

namespace pxl
{
    OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int count, unsigned int* data)
    : IndexBuffer(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    }

    void OpenGLIndexBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLIndexBuffer::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}