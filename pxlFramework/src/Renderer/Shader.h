#pragma once

#include <glm/mat4x4.hpp>

#include "RendererAPIType.h"
#include "GraphicsContext.h"

namespace pxl
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Reload() = 0;

        virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0; // not api-agnostic
        virtual void SetUniformInt1(const std::string& name, int value) = 0;

        static std::shared_ptr<Shader> Create(const std::string& vertSrc, const std::string& fragSrc);
        static std::shared_ptr<Shader> Create(const std::shared_ptr<GraphicsContext>& graphicsContext, const std::vector<char>& vertBin, const std::vector<char>& fragBin);
        static std::shared_ptr<Shader> Create(RendererAPIType api, const std::shared_ptr<GraphicsContext>& graphicsContext, const std::vector<char>& vertBin, const std::vector<char>& fragBin);
    };
}