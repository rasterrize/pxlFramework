#include "Shader.h"
#include "Renderer.h"

#include "OpenGL/OpenGLShader.h"

namespace pxl
{
    std::shared_ptr<Shader> Shader::Create(const std::string vertSrc, const std::string fragSrc)
    {
        switch (Renderer::GetAPIType())
        {
            case RendererAPIType::None:
                Logger::LogError("Can't create Shader for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLShader>(vertSrc, fragSrc);
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't create Shader for Vulkan renderer api.");
                break;
        }
        
        return nullptr;
    }
}
