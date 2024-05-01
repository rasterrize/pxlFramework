#pragma once

#include "../Texture.h"

#include <glm/vec2.hpp>

namespace pxl
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(unsigned char* imageBuffer, glm::vec2 imageSize, int channels);
        virtual ~OpenGLTexture2D() override;

        virtual void Bind() override;
        virtual void Unbind() override;
    private:
        unsigned char* m_ImageBuffer;
        glm::vec2 m_ImageSize;
        int m_Channels;

        uint32_t m_RendererID;
    };
}