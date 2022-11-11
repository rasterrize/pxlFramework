#include "../Shader.h"

namespace pxl
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertSrc, const std::string& fragSrc);
        ~OpenGLShader();

        virtual void Bind() override;
        virtual void Unbind() override;
    private:
        unsigned int m_RendererID;
    };
}