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

    struct TextureSpecs
    {
        TextureType Type = TextureType::Tex2D;
        TextureWrap WrapMode = TextureWrap::ClampToEdge;
        SampleFilter Filter = SampleFilter::Linear;

        // -1.0 will use max anisotropy defined by renderer/device
        float MaxAnisotropy = -1.0f;
    };

    /// @brief An image stored in the GPU's memory which can be used in the graphics pipeline.
    class Texture : public GPUResource
    {
    public:
        virtual ~Texture() = default;

        virtual void Free() override = 0;

        virtual void SetData(const void* data) = 0;
    };
}