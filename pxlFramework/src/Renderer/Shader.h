#pragma once

#include <glm/glm.hpp>

namespace pxl
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0; // not api-agnostic
    };
}