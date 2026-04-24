#pragma once

#include "Core/Image.h"
#include "GPUResource.h"

namespace pxl
{
    enum class TextureType
    {
        Tex1D,
        Tex2D,
        Tex3D,
    };

    enum class TextureFormat
    {
        RGBA8,
    };

    enum class TextureWrap
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
    };

    enum class SampleFilter
    {
        Nearest,
        Linear,
    };

    struct TextureSpecs
    {
        std::shared_ptr<Image> Image;
        TextureType Type = TextureType::Tex2D;
        TextureFormat Format = TextureFormat::RGBA8;
        TextureWrap WrapMode = TextureWrap::ClampToEdge;
        SampleFilter Filter = SampleFilter::Linear;
        bool UseAnistropicFiltering = false;

        // Any value below 1.0 means it will be auto overridden by the renderer's desired anisotropy level
        float AnisotropyLevel = 0.0f;
    };

    struct TextureMetadata
    {
        Size2D Size = {};
    };

    /// @brief An image stored in the GPU's memory which can be used for rendering in a graphics pipeline.
    class Texture : public GPUResource
    {
    public:
        virtual ~Texture() = default;

        virtual void SetData(const void* data) = 0;

        virtual void SetAnisotropicLevel(float level) = 0;

        virtual const TextureSpecs& GetSpecs() const = 0;

        virtual const TextureMetadata& GetMetadata() const = 0;
    };

    struct SubTexture
    {
        std::weak_ptr<Texture> Texture;
        std::array<glm::vec2, 4> Coords;
    };

    namespace Utils
    {
        inline uint32_t ToNumOfChannels(TextureFormat format)
        {
            switch (format)
            {
                case TextureFormat::RGBA8: return 4;
                default:                   return 0;
            }
        }

        inline std::string ToString(TextureType type)
        {
            switch (type)
            {
                case TextureType::Tex1D: return "1D";
                case TextureType::Tex2D: return "2D";
                case TextureType::Tex3D: return "3D";
                default:                 return "Unknown";
            }
        }

        inline std::string ToString(TextureFormat format)
        {
            switch (format)
            {
                case TextureFormat::RGBA8: return "RGBA8";
                default:                   return "Unknown";
            }
        }

        inline std::string ToString(TextureWrap wrap)
        {
            switch (wrap)
            {
                case TextureWrap::Repeat:         return "Repeat";
                case TextureWrap::MirroredRepeat: return "MirroredRepeat";
                case TextureWrap::ClampToEdge:    return "ClampToEdge";
                case TextureWrap::ClampToBorder:  return "ClampToBorder";
                default:                          return "Unknown";
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