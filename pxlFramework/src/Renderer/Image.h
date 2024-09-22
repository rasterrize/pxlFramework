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

    struct ImageMetadata
    {
        Size2D Size = Size2D(0);
        ImageFormat Format = ImageFormat::Undefined;
        uint32_t BitDepth = 8;
    };

    struct Image
    {
        unsigned char* Buffer = nullptr;
        ImageMetadata Metadata = {};
    };
}