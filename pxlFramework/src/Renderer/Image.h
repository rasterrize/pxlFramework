#pragma once

namespace pxl
{
    enum class ImageFormat
    {
        Undefined = 0,
        RGB8,
        RGBA8
    };

    enum class ImageFilter // Should this be TextureFilter instead?
    {
        Undefined = 0,
        Nearest,           // Should nearest be default?
        Linear
    };

    struct Image
    {
        unsigned char* Buffer;
        glm::vec2 Size; // ivec?
        ImageFormat Format;
    };
}