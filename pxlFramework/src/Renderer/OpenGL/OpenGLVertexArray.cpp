#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID); 
        // Bind Vertex Array ? 
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind()
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind()
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::SetLayout(const BufferLayout& layout)
    {
        glBindVertexArray(m_RendererID);
        uint32_t index = 0; // attribute number
        size_t offset = 0; // amount of bytes currently allocated
        for (const BufferElement& element : layout.GetElements())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, element.Count, element.GetOpenGLType(), element.Normalized, layout.GetStride(), (const void*)offset);
            offset += element.Count * element.GetSizeOfType();
            index++;
        }
    }
}