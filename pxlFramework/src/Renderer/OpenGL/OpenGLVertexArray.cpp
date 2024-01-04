#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID); 
        glBindVertexArray(m_RendererID);
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
            glVertexAttribPointer(index, element.Count, GetOpenGLTypeOfBufferDataType(element.Type), element.Normalized, layout.GetStride(), (const void*)offset);
            offset += element.Count * GetSizeOfType(element.Type);
            index++;
        }
    }

    GLenum OpenGLVertexArray::GetOpenGLTypeOfBufferDataType(BufferDataType type)
    {
        switch (type)
        {
            case BufferDataType::Float:  return GL_FLOAT;
            case BufferDataType::Float2: return GL_FLOAT;
            case BufferDataType::Float3: return GL_FLOAT;
            case BufferDataType::Float4: return GL_FLOAT;
            case BufferDataType::Int:    return GL_INT;
            case BufferDataType::Int2:   return GL_INT;
            case BufferDataType::Int3:   return GL_INT;
            case BufferDataType::Int4:   return GL_INT;
            case BufferDataType::Bool:   return GL_BOOL;
        }
        return 0;
    }
}