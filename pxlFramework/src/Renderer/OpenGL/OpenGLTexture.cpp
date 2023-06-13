#include "OpenGLTexture.h"

#include <glad/glad.h>

namespace pxl
{
    OpenGLTexture::OpenGLTexture(unsigned char* imageBuffer, glm::vec2 imageSize, int channels)
        : m_ImageBuffer(imageBuffer), m_ImageSize(imageSize), m_Channels(channels)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_ImageSize.x, m_ImageSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageBuffer);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture::Bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLTexture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}