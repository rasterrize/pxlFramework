#include "../Texture.h"
#include <glm/glm.hpp>

namespace pxl
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture::OpenGLTexture(unsigned char* imageBuffer, glm::vec2 imageSize, int channels);

        virtual void Bind() override;
        virtual void Unbind() override;
    private:
        unsigned char* m_ImageBuffer;
        glm::vec2 m_ImageSize;
        int m_Channels;

        unsigned int m_RendererID;
    };
}