#include "OpenGLTexture.h"

namespace pxl
{
    OpenGLTexture::OpenGLTexture(const Image& image)
        : m_Metadata(image.Metadata), m_GLFormat(ImageFormatToGLFormat(image.Metadata.Format))
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<int>(m_Metadata.Size.Width), static_cast<int>(m_Metadata.Size.Height), 0, m_GLFormat, GL_UNSIGNED_BYTE, image.Buffer);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    OpenGLTexture::~OpenGLTexture()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture::Bind(uint32_t unit)
    {
        glBindTextureUnit(unit, m_RendererID);
    }

    void OpenGLTexture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLenum OpenGLTexture::ImageFormatToGLFormat(ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined: return GL_INVALID_ENUM;
            case ImageFormat::RGB8:      return GL_RGB;
            case ImageFormat::RGBA8:     return GL_RGBA;
        }

        return GL_INVALID_ENUM;
    }
}