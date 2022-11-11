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

    void OpenGLVertexArray::SetLayout(const BufferLayout& layout)
    {
        glBindVertexArray(m_RendererID);
        unsigned int index = 0;
        unsigned int offset = 0;
        for (BufferElement element : layout.GetElements())
        {
            //Logger::LogInfo(std::to_string(index));
            //offset += element.Count * element.GetSizeOfType();
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, element.Count, element.GetOpenGLType(), element.Normalized, 0, (void*)0); // FIX STRIDE
            offset += 
            index++;
        }
    }
}