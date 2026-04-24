#pragma once

#include "Texture.h"

namespace pxl
{
    class TextureHandler : public GPUResource
    {
    public:
        virtual ~TextureHandler() = default;

        virtual void Add(std::shared_ptr<Texture> texture) = 0;

        virtual uint32_t GetIndex(const std::shared_ptr<Texture>& texture) = 0;

        virtual void Upload() = 0;

        virtual bool NeedsUpload() const = 0;

        virtual bool IsFull() const = 0;

        virtual void Reset() = 0;
    };
}