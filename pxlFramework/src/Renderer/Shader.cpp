#include "Shader.h"
#include "Renderer.h"
#include "OpenGL/OpenGLShader.h"

namespace pxl
{
    static Shader* Create(const std::string& vertSrc, const std::string& fragSrc)
    {
        switch (Renderer::GetRendererAPIType())
        {
            case RendererAPIType::OpenGL:
                return new OpenGLShader(vertSrc, fragSrc);
            break;
            case RendererAPIType::Vulkan:
            break;
            case RendererAPIType::DirectX12:
            break;
        }

        return nullptr;
    }
}