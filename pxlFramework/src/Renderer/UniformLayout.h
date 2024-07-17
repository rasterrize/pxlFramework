#pragma once

#include "BufferLayout.h"
#include "Shader.h"

namespace pxl
{
    struct UniformElement
    {
        std::string Name;
        BufferDataType Type;
        ShaderStage ShaderStage;
    };

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
                bytes += SizeOfBufferDataType(element.Type);
            }

            return bytes;
        }

    private:
        std::vector<UniformElement> m_Elements;
    };

    struct PushConstantElement
    {
        std::string Name;
        BufferDataType Type;
        ShaderStage PushConstantShaderStage;
    };

    class PushConstantLayout
    {
    public:
        const std::vector<PushConstantElement> GetElements() const { return m_Elements; }

        void Add(const PushConstantElement& pc)
        {
            m_Elements.push_back(pc);
        }

    private:
        std::vector<PushConstantElement> m_Elements;
    };
}