#pragma once

#include "Image.h"

namespace pxl
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t unit) = 0; // unsure if this is used in Vulkan
        virtual void Unbind() = 0;

        static std::shared_ptr<Texture> Create(const Image& image);
    };
}