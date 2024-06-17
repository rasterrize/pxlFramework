#include "OpenGLVertexArray.h"

namespace pxl
{
    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID); 
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

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<Buffer>& vertexBuffer, const BufferLayout& layout)
    {
        glBindVertexArray(m_RendererID); // Ensure this vertex array is the one currently bound
        vertexBuffer->Bind(); // Sets the global vertex buffer bound so glVertexAttribPointer can grab it and store it in the VAO
        
        uint32_t index = 0; // attribute number (location in shader)
        size_t offset = 0; // amount of bytes currently allocated
        for (const BufferElement& element : layout.GetElements())
        {
            glEnableVertexAttribArray(index);

            // Float type checking
            if (element.Type == BufferDataType::Float || element.Type == BufferDataType::Float2 || element.Type == BufferDataType::Float3 || element.Type == BufferDataType::Float4)
                glVertexAttribPointer(index, element.CountOfBufferDataType(), GetOpenGLTypeOfBufferDataType(element.Type), element.Normalized, layout.GetStride(), reinterpret_cast<void*>(offset));

            // Int type checking
            if (element.Type == BufferDataType::Int || element.Type == BufferDataType::Int2 || element.Type == BufferDataType::Int3 || element.Type == BufferDataType::Int4)
                glVertexAttribIPointer(index, element.CountOfBufferDataType(), GetOpenGLTypeOfBufferDataType(element.Type), layout.GetStride(), reinterpret_cast<void*>(offset));

            // Bool type checking
            if (element.Type == BufferDataType::Bool)
                glVertexAttribIPointer(index, element.CountOfBufferDataType(), GetOpenGLTypeOfBufferDataType(element.Type), layout.GetStride(), reinterpret_cast<void*>(offset));

            // Mat type checking
            if (element.Type == BufferDataType::Mat3 || element.Type == BufferDataType::Mat4)
                glVertexAttribPointer(index, element.CountOfBufferDataType(), GetOpenGLTypeOfBufferDataType(element.Type), element.Normalized, layout.GetStride(), reinterpret_cast<void*>(offset));
                            
            offset += SizeOfBufferDataType(element.Type);
            index++;
        }
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<Buffer>& indexBuffer)
    {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();
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