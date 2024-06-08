#pragma once

#include "../Texture.h"

#include <glm/vec2.hpp>
#include <glad/glad.h>

namespace pxl
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const Image& image);
        virtual ~OpenGLTexture2D() override;

        virtual void Bind(uint32_t unit) override;
        virtual void Unbind() override;

        static GLenum ImageFormatToGLFormat(ImageFormat format);
    private:
        unsigned char* m_ImageBuffer;
        glm::vec2 m_ImageSize;
        GLenum m_GLFormat;

        uint32_t m_RendererID;
    };
}