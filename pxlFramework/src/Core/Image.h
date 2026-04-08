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

    enum class ImageFileFormat
    {
        JPG,
        PNG,
        BMP,
    };

    struct ImageMetadata
    {
        Size2D Size = {};
        ImageFormat Format = ImageFormat::Undefined;
    };

    struct Image
    {
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

        inline uint32_t ToNumOfChannels(ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::Undefined: return 0;
                case ImageFormat::RGB8:      return 3;
                case ImageFormat::RGBA8:     return 4;
                default:                     return 0;
            }
        }

        inline ImageFormat ToImageFormat(uint32_t channels)
        {
            switch (channels)
            {
                case 3:  return ImageFormat::RGB8;
                case 4:  return ImageFormat::RGBA8;
                default: return ImageFormat::Undefined;
            }
        }
    }
}