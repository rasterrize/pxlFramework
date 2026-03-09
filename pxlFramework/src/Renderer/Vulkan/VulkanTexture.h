#pragma once

#include "Renderer/Texture.h"

namespace pxl
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture();

        virtual void Free() override;

        virtual void SetData(const void* data) override;
    };
}