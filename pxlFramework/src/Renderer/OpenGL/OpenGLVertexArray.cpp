#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind()
    {
        glBindVertexArray(m_RendererID);
        m_VertexBuffer->Bind();
        m_IndexBuffer->Bind();
    }

    void OpenGLVertexArray::Unbind()
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::SetLayout(BufferLayout& layout)
    {
        glBindVertexArray(m_RendererID);
        unsigned int index = 0; // attribute number
        unsigned int offset = 0; // amount of bytes currently allocated
        for (BufferElement element : layout.GetElements())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, element.Count, element.GetOpenGLType(), element.Normalized, layout.GetStride(), (const void*)offset); // TODO: change this to use the offsetoff() function
            offset += element.Count * element.GetSizeOfType();
            index++;
        }
    }
}