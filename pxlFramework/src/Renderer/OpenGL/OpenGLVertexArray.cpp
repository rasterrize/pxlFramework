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
    }

    void OpenGLVertexArray::Unbind()
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::SetLayout(BufferLayout& layout)
    {
        glBindVertexArray(m_RendererID); // should this be binding only the vertex buffer instead of binding the va which binds vb and ib aswell?
        unsigned int index = 0;
        unsigned int offset = 0;
        for (BufferElement element : layout.GetElements())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, element.Count, element.GetOpenGLType(), element.Normalized, layout.GetStride(), (const void*)offset);
            offset += element.Count * element.GetSizeOfType();
            index++;
        }
    }
}