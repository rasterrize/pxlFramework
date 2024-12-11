#include "Shader.h"

#include "OpenGL/OpenGLShader.h"
#include "Renderer.h"
#include "Vulkan/VulkanShader.h"

namespace pxl
{
    std::shared_ptr<Shader> Shader::Create(ShaderStage stage, const std::string& glslSrc)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Shader for no renderer api.");
                break;

            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLShader>(stage, glslSrc);

            case RendererAPIType::Vulkan:
                return std::make_shared<VulkanShader>(stage, glslSrc);
        }

        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(ShaderStage stage, const std::vector<char>& sprvBin)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Shader for no renderer api.");
                break;

            case RendererAPIType::OpenGL:
                PXL_LOG_ERROR(LogArea::Renderer, "OpenGL implementation doesn't support shaders from binary.");
                break;

            case RendererAPIType::Vulkan:
                return std::make_shared<VulkanShader>(stage, sprvBin);
        }

        return nullptr;
    }
}
