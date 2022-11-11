#include "../VertexBuffer.h"
#include "../BufferLayout.h"

namespace pxl
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(int size, const void* data);

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual const BufferLayout GetLayout() override { return m_Layout; }

        virtual void AddLayout(const BufferLayout& layout) override { m_Layout = layout; }
    private:
        unsigned int m_RendererID;
        BufferLayout m_Layout;
    };
}