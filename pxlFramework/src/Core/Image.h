#pragma once

#include <stb_image.h>

namespace pxl
{
    enum class ImageFormat
    {
        Undefined,
        RGB8,
        RGBA8,
    };

    enum class SampleFilter
    {
        Nearest,
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

    namespace Utils
    {
        inline std::string ToString(ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::Undefined: return "Undefined";
                case ImageFormat::RGB8:      return "RGB8";
                case ImageFormat::RGBA8:     return "RGBA8";
                default:                     return "Unknown";
            }
        }

        inline std::string ToString(SampleFilter filter)
        {
            switch (filter)
            {
                case SampleFilter::Nearest: return "Nearest";
                case SampleFilter::Linear:  return "Linear";
                default:                    return "Unknown";
            }
        }
    }
}