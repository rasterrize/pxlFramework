#pragma once

#include "Core/Image.h"

namespace pxl
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t unit) = 0; // unsure if this is used in Vulkan
        virtual void Unbind() = 0;

        virtual void SetData(const void* data) = 0;

        virtual const ImageMetadata& GetMetadata() const = 0;

        static std::shared_ptr<Texture> Create(const Image& image);
        static std::shared_ptr<Texture> Create(const std::shared_ptr<Image>& image);

        static std::shared_ptr<Texture> CreateErrorTexture(); 
    };
}