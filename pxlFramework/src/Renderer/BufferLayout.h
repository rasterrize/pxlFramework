#pragma once

#include <glad/glad.h>

namespace pxl
{
    enum class BufferDataType
    {
        None = 0, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Mat3, Mat4, Bool
    };

    struct BufferElement
    {
        unsigned int Count; // How many variables/data of the specified data type (eg. the amount/count of floats in the element)
        BufferDataType Type; // The type of data (float, int, etc)
        bool Normalized;

        unsigned int GetSizeOfType()
        {
            switch (Type)
            {
                case BufferDataType::Float: return 4;
            }
            return 0;
        }

        GLenum GetOpenGLType()
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
        unsigned int GetStride() { CalculateStride(); return m_Stride; }

        void Add(unsigned int count, BufferDataType type, bool normalized)
        {
            m_Elements.push_back({count, type, normalized});
        }

        void Add(const BufferElement& element)
        {
            m_Elements.push_back(element);
        }

        void CalculateStride()
        {
            unsigned int stride = 0;
            for (BufferElement element : m_Elements)
            {
                stride += element.Count * element.GetSizeOfType();
            }
            m_Stride = stride;
            Logger::LogInfo("Calculated Stride: " + m_Stride);
        }
    private:
        std::vector<BufferElement> m_Elements;
        unsigned int m_Stride; // Stride is the size of the entire buffer (eg. Vertex Buffer with all of it's attributes (positions, tex coords))
    };
}