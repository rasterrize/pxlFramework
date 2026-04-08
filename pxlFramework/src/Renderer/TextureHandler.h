#pragma once

#include "Texture.h"

namespace pxl
{
    class TextureHandler : public GPUResource
    {
    public:
        virtual ~TextureHandler() = default;

        /// @brief Adds a texture to the list of handled textures to be sent to the shader
        /// @return The texture index in array of textures
        virtual uint32_t Add(std::shared_ptr<Texture> texture) = 0;

        virtual bool IsFull() const = 0;

        virtual void Upload() = 0;

        virtual void Reset() = 0;
    };
}