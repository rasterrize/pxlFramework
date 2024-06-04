#pragma once

#include "Image.h"

namespace pxl
{
    class Texture2D
    {
    public:
        virtual ~Texture2D() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        static std::shared_ptr<Texture2D> Create(const Image& image);
    };
}