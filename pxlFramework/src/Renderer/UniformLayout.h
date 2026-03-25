#pragma once

#include "BufferLayout.h"
#include "Shader.h"

namespace pxl
{
    enum class UniformDataType
    {
        None,
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        Mat3,
        Mat4,
        Bool,
        IntArray,
    };

    // TODO: move this outside of global space
    // Returns size of type in bytes
    static constexpr uint32_t SizeOfUniformDataType(UniformDataType type)
    {
        switch (type)
        {
            // These are currently hardcoded but an optimal way in the future would be to use API data types, such as sizeof(Glfloat)
            case UniformDataType::Float:  return 4;
            case UniformDataType::Float2: return 4 * 2;
            case UniformDataType::Float3: return 4 * 3;
            case UniformDataType::Float4: return 4 * 4;
            case UniformDataType::Int:    return 4;
            case UniformDataType::Int2:   return 4 * 2;
            case UniformDataType::Int3:   return 4 * 3;
            case UniformDataType::Int4:   return 4 * 4;
            case UniformDataType::Mat3:   return 4 * 3 * 3;
            case UniformDataType::Mat4:   return 4 * 4 * 4;
        }
        return 0;
    }

    struct UniformElement
    {
        std::string Name;
        UniformDataType Type = UniformDataType::None;
        ShaderStage ShaderStage = ShaderStage::Vertex;
    };

    /// @brief Describes the layout of uniform variables used in a pipeline
    class UniformLayout
    {
    public:
        const std::vector<UniformElement> GetElements() const { return m_Elements; }

        void Add(const UniformElement& element)
        {
            m_Elements.push_back(element);
        }

        uint32_t GetSize()
        {
            uint32_t bytes = 0;
            for (const auto& element : m_Elements)
            {
                bytes += SizeOfUniformDataType(element.Type);
            }

            return bytes;
        }

    private:
        std::vector<UniformElement> m_Elements;
    };

    namespace Utils
    {
        inline std::string ToString(UniformDataType type)
        {
            switch (type)
            {
                case UniformDataType::None:     return "None";
                case UniformDataType::Float:    return "Float";
                case UniformDataType::Float2:   return "Float2";
                case UniformDataType::Float3:   return "Float3";
                case UniformDataType::Float4:   return "Float4";
                case UniformDataType::Int:      return "Int";
                case UniformDataType::Int2:     return "Int2";
                case UniformDataType::Int3:     return "Int3";
                case UniformDataType::Int4:     return "Int4";
                case UniformDataType::Mat3:     return "Mat3";
                case UniformDataType::Mat4:     return "Mat4";
                case UniformDataType::Bool:     return "Bool";
                case UniformDataType::IntArray: return "IntArray";
                default:                        return "Unknown";
            }
        }
    }
}