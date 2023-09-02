#pragma once

#include <glad/glad.h>

namespace pxl
{
    enum class BufferDataType
    {
        None = 0, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Mat3, Mat4, Bool
    };

    struct BufferElement // is this really a struct if it has member functions?
    {
        uint32_t Count; // How many variables/data of the specified data type (eg. the amount/count of floats in the element)
        BufferDataType Type; // The type of data (float, int, etc)
        bool Normalized;

        uint32_t GetSizeOfType() const
        {
            switch (Type)
            {
                case BufferDataType::Float: return 4;
            }
            return 0;
        }

        GLenum GetOpenGLType() const // opengl code shouldnt be here
        {
            switch (Type)
            {
                case BufferDataType::Float: return GL_FLOAT;
            }
            return 0;
        }
    };

    class BufferLayout
    {
    public:
        std::vector<BufferElement> GetElements() const { return m_Elements; }
        const uint32_t GetStride() const { return m_Stride; }

        void Add(uint32_t count, BufferDataType type, bool normalized)
        {
            BufferElement element = { count, type, normalized };
            m_Elements.push_back(element);
            m_Stride += element.Count * element.GetSizeOfType();
        }

        void Add(BufferElement& element)
        {
            m_Elements.push_back(element);
            m_Stride += element.Count * element.GetSizeOfType();
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0; // Stride is the size of the entire buffer (eg. Vertex Buffer with all of it's attributes (positions, tex coords))
    };
}