#pragma once

namespace pxl
{
    enum class BufferDataType
    {
        None = 0, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Mat3, Mat4, Bool
    };

    // Returns size of type in bytes
    static uint32_t SizeOfBufferDataType(BufferDataType type)
    {
        switch (type)
        {
            // These are currently hardcoded but an optimal way in the future would be to use API data types, such as sizeof(Glfloat)
            case BufferDataType::Float:  return 4;
            case BufferDataType::Float2: return 4 * 2;
            case BufferDataType::Float3: return 4 * 3;
            case BufferDataType::Float4: return 4 * 4;
            case BufferDataType::Int:    return 4;
            case BufferDataType::Int2:   return 4 * 2;
            case BufferDataType::Int3:   return 4 * 3;
            case BufferDataType::Int4:   return 4 * 4;
            case BufferDataType::Mat3:   return 4 * 3 * 3;
            case BufferDataType::Mat4:   return 4 * 4 * 4;
        }
        return 0;
    }

    struct BufferElement
    {
        BufferDataType Type;
        bool Normalized;

        uint32_t CountOfBufferDataType() const
        {
            switch (Type)
            {
                case BufferDataType::Float:  return 1;
                case BufferDataType::Float2: return 2;
                case BufferDataType::Float3: return 3;
                case BufferDataType::Float4: return 4;
                case BufferDataType::Int:    return 1;
                case BufferDataType::Int2:   return 2;
                case BufferDataType::Int3:   return 3;
                case BufferDataType::Int4:   return 4;
                case BufferDataType::Mat3:   return 3; // } unsure about these two
                case BufferDataType::Mat4:   return 4; // }
            }
            return 0;
        }
    };

    class BufferLayout
    {
    public:
        std::vector<BufferElement> GetElements() const { return m_Elements; }
        const uint32_t GetStride() const { return m_Stride; }

        void Add(const BufferElement& element)
        {
            m_Elements.push_back(element);
            m_Stride += SizeOfBufferDataType(element.Type);
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0; // Stride is the size of the entire buffer layout in bytes (eg. Vertex Buffer with all of it's attributes (positions, tex coords))
    };
}