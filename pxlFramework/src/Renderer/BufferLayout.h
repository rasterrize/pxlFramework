#pragma once

namespace pxl
{
    /// @brief Specifies a type of data typically stored in a GPUBuffer
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
        Bool = Int,
    };

    namespace Utils
    {
        inline constexpr uint32_t SizeOfBufferDataType(BufferDataType type)
        {
            switch (type)
            {
                case BufferDataType::Float: return sizeof(float);
                case BufferDataType::Vec2:  return sizeof(float) * 2;
                case BufferDataType::Vec3:  return sizeof(float) * 3;
                case BufferDataType::Vec4:  return sizeof(float) * 4;
                case BufferDataType::Int:   return sizeof(int);
                case BufferDataType::IVec2: return sizeof(int) * 2;
                case BufferDataType::IVec3: return sizeof(int) * 3;
                case BufferDataType::IVec4: return sizeof(int) * 4;
                case BufferDataType::Mat3:  return sizeof(float) * 3 * 3;
                case BufferDataType::Mat4:  return sizeof(float) * 4 * 4;
                default:                    return 0;
            }
        }

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
                default:                    return "Unknown";
            }
        }
    }

    /// @brief Describes the layout of data in a GPUBuffer.
    struct BufferLayout
    {
        std::vector<BufferDataType> Elements;

        uint32_t GetStride() const
        {
            uint32_t stride = 0;
            for (const auto& element : Elements)
                stride += Utils::SizeOfBufferDataType(element);

            return stride;
        }
    };
}