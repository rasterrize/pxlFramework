#pragma once

#include <glad/glad.h>

#include <glm/vec2.hpp>

#include "Renderer/Texture.h"

namespace pxl
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const Image& image, const TextureSpecs& specs);
        OpenGLTexture(const std::shared_ptr<Image>& image, const TextureSpecs& specs);
        virtual ~OpenGLTexture() override;

        virtual void SetData(const void* data) override;

        virtual void Bind(uint32_t unit) override;
        virtual void Unbind() override;

        virtual const ImageMetadata& GetMetadata() const override { return m_Metadata; }

    private:
        void CreateTexture(const std::vector<uint8_t>& pixels);

    private:
        static GLenum ToGLFormat(ImageFormat format);
        static GLenum ToGLFilter(SampleFilter filter);
        static GLenum ToGLType(TextureType type);
        static GLenum ToGLWrapMode(TextureWrap mode);

    private:
        ImageMetadata m_Metadata;
        TextureSpecs m_Specs;

        uint32_t m_RendererID = 0;
    };
}