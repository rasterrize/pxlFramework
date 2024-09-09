#pragma once

#include "glm/vec2.hpp"

namespace pxl
{
    enum class ImageFormat
    {
        Undefined,
        RGB8,
        RGBA8,
    };

    enum class ImageFilter
    {
        Undefined,
        Nearest, // Should nearest be default?
        Linear,
    };

    struct Image
    {
        unsigned char* Buffer = nullptr;
        glm::vec2 Size = glm::vec2(0.0f); // ivec?
        ImageFormat Format = ImageFormat::Undefined;
    };
}