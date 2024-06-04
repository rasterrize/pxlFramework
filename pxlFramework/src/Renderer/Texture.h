#pragma once

#include "Image.h"

namespace pxl
{
    class Texture2D
    {
    public:
        virtual ~Texture2D() = default;

        virtual void Bind(uint32_t unit) = 0; // unsure if this is used in Vulkan
        virtual void Unbind() = 0;

        static std::shared_ptr<Texture2D> Create(const Image& image);
    };
}