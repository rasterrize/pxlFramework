#pragma once

#include <glad/glad.h>

#include <glm/vec2.hpp>

#include "Renderer/Texture.h"

namespace pxl
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const Image& image);
        virtual ~OpenGLTexture() override;

        virtual void Bind(uint32_t unit) override;
        virtual void Unbind() override;

        virtual const ImageMetadata& GetMetadata() const override { return m_Metadata; }
    private:
        static GLenum ImageFormatToGLFormat(ImageFormat format);
    private:
        ImageMetadata m_Metadata;

        GLenum m_GLFormat = GL_INVALID_ENUM;
        uint32_t m_RendererID = 0;
    };
}