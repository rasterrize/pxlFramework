#include "Shader.h"
#include "Renderer.h"

#include "OpenGL/OpenGLShader.h"
#include "Vulkan/VulkanShader.h"

namespace pxl
{
    std::shared_ptr<Shader> Shader::Create(const std::string& vertSrc, const std::string& fragSrc)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Shader for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLShader>(vertSrc, fragSrc);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Vulkan implementation doesn't support shaders from strings");
                break;
        }
        
        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(const std::vector<char>& vertBin, const std::vector<char>& fragBin)
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
                return std::make_shared<VulkanShader>(std::dynamic_pointer_cast<VulkanDevice>(Renderer::GetCurrentDevice()), vertBin, fragBin);
        }
        
        return nullptr;
    }
}
