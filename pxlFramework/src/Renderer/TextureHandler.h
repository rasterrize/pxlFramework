#pragma once

#include "GraphicsPipeline.h"
#include "Texture.h"

namespace pxl
{
    class TextureHandler
    {
    public:
        virtual float UseTexture(const std::shared_ptr<Texture>& texture) = 0;

        virtual void BindTextures() = 0;

        virtual void UploadShaderData(const std::shared_ptr<GraphicsPipeline>& pipeline) = 0;

        virtual bool IsFull() const = 0;
    };
}