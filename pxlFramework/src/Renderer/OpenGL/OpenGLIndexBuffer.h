#include "../IndexBuffer.h"

namespace pxl
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(unsigned int count, unsigned int* data);

        virtual void Bind() override;
        virtual void Unbind() override;

    private:
        unsigned int m_RendererID;
    };
}