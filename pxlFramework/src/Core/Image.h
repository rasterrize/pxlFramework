#pragma once

#include <stb_image.h>

#include <glm/vec2.hpp>

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

    enum class ImageFileFormat
    {
        JPG,
        PNG,
        BMP,
    };

    struct ImageMetadata
    {
        Size2D Size = Size2D(0);
        ImageFormat Format = ImageFormat::Undefined;
        uint32_t BitDepth = 8;
    };

    struct Image
    {
        Image() = default;

        Image(unsigned char* bytePtr, Size2D size, uint32_t channels)
        {
            Buffer = std::vector<uint8_t>(bytePtr, bytePtr + size.Width * size.Height * channels);
            Metadata.Size = size;

            // Move this to Utils
            switch (channels)
            {
                case 3: Metadata.Format = ImageFormat::RGB8; break;
                case 4: Metadata.Format = ImageFormat::RGBA8; break;
            }
        }

        Image(std::vector<uint8_t> pixels, Size2D size, ImageFormat format)
            : Buffer(pixels), Metadata({ size, format })
        {
        }

        /* TODO: It might make more sense to arrange this data in uint32_t's. but then we have to take care of images (jpg's) without alpha values. */
        std::vector<uint8_t> Buffer;
        ImageMetadata Metadata = {};
    };
}