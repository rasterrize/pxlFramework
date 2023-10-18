#pragma once

#include "../Texture.h"

#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLTexture2D : public Texture
    {
    public:
        OpenGLTexture2D(unsigned char* imageBuffer, glm::vec2 imageSize, int channels);
        ~OpenGLTexture2D();

        virtual void Bind() override;
        virtual void Unbind() override;
    private:
        unsigned char* m_ImageBuffer;
        glm::vec2 m_ImageSize;
        int m_Channels;

        //uint32_t m_Rows;

        uint32_t m_RendererID;
    };
}