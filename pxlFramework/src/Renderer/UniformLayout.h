#pragma once

#include "BufferLayout.h"
#include "Shader.h"

namespace pxl
{
    struct UniformElement
    {
        std::string Name;
        BufferDataType Type;
        bool IsPushConstant;
        ShaderStage PushConstantShaderStage;
    };

    class UniformLayout
    {
    public:
        const std::vector<UniformElement> GetElements() const { return m_Elements; }

        void Add(const UniformElement& element)
        {
            m_Elements.push_back(element);
        }

    private:
        std::vector<UniformElement> m_Elements;
    };
}