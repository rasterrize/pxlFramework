#pragma once

namespace pxl
{
    enum class BufferDataType
    {
        None,
        Float,
        Vec2,
        Vec3,
        Vec4,
        Int,
        IVec2,
        IVec3,
        IVec4,
        Mat3,
        Mat4,
        Bool,
    };

    namespace Utils
    {
        /// @brief Gets the size of a buffer data type in bytes
        /// @param type The BufferDataType
        /// @return The number of bytes
        inline constexpr uint32_t SizeOfBufferDataType(BufferDataType type)
        {
            switch (type)
            {
                case BufferDataType::Float: return 4;
                case BufferDataType::Vec2:  return 4 * 2;
                case BufferDataType::Vec3:  return 4 * 3;
                case BufferDataType::Vec4:  return 4 * 4;
                case BufferDataType::Int:   return 4;
                case BufferDataType::IVec2: return 4 * 2;
                case BufferDataType::IVec3: return 4 * 3;
                case BufferDataType::IVec4: return 4 * 4;
                case BufferDataType::Mat3:  return 4 * 3 * 3;
                case BufferDataType::Mat4:  return 4 * 4 * 4;
                case BufferDataType::Bool:  return 4;
                default:                    return 0;
            }
        }
    }

    /// @brief Describes the layout of data in a GPUBuffer
    class BufferLayout
    {
    public:
        const std::vector<BufferDataType>& GetElements() const { return m_Elements; }
        uint32_t GetStride() const { return m_Stride; }

        constexpr void Add(BufferDataType element)
        {
            m_Elements.push_back(element);
            m_Stride += Utils::SizeOfBufferDataType(element);
        }

    private:
        std::vector<BufferDataType> m_Elements;

        // Stride is the size of the entire buffer layout in bytes (eg. Vertex Buffer with all of it's attributes (positions, tex coords))
        uint32_t m_Stride = 0;
    };

    namespace Utils
    {
        inline std::string ToString(BufferDataType type)
        {
            switch (type)
            {
                case BufferDataType::None:  return "None";
                case BufferDataType::Float: return "Float";
                case BufferDataType::Vec2:  return "Vec2";
                case BufferDataType::Vec3:  return "Vec3";
                case BufferDataType::Vec4:  return "Vec4";
                case BufferDataType::Int:   return "Int";
                case BufferDataType::IVec2: return "IVec2";
                case BufferDataType::IVec3: return "IVec3";
                case BufferDataType::IVec4: return "IVec4";
                case BufferDataType::Mat3:  return "Mat3";
                case BufferDataType::Mat4:  return "Mat4";
                case BufferDataType::Bool:  return "Bool";
                default:                    return "Unknown";
            }
        }
    }
}