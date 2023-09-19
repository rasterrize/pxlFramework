#pragma once

#include "../Texture.h"

#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(unsigned char* imageBuffer, glm::vec2 imageSize, int channels);
        ~OpenGLTexture();

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