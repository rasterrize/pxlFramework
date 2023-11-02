#pragma once

#include <glm/vec2.hpp>

namespace pxl
{
    class Texture2D
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        static std::shared_ptr<Texture2D> Create(unsigned char* imageBuffer, const glm::vec2& imageSize, int channels);
    };
}