#pragma once

#include <glm/glm.hpp>

namespace pxl
{
    class Texture
    {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        static std::shared_ptr<Texture> Create(unsigned char* imageBuffer, glm::vec2 imageSize, int channels);
    };
}