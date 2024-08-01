#pragma once

#include "Renderer/Texture.h"

#include <glm/vec2.hpp>
#include <glad/glad.h>

namespace pxl
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const Image& image);
        virtual ~OpenGLTexture() override;

        virtual void Bind(uint32_t unit) override;
        virtual void Unbind() override;

        static GLenum ImageFormatToGLFormat(ImageFormat format);
    private:
        unsigned char* m_ImageBuffer;
        glm::u32vec2 m_ImageSize;
        GLenum m_GLFormat;

        uint32_t m_RendererID;
    };
}