#include "Shader.h"
#include "Renderer.h"

#include "OpenGL/OpenGLShader.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"

namespace pxl
{
    std::shared_ptr<Shader> Shader::Create(const std::string& vertSrc, const std::string& fragSrc)
    {
        switch (Renderer::GetAPIType())
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

    std::shared_ptr<Shader> Shader::Create(const std::shared_ptr<GraphicsContext>& graphicsContext, const std::vector<char>& vertBin, const std::vector<char>& fragBin)
    {
        switch (Renderer::GetAPIType())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Shader for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                PXL_LOG_ERROR(LogArea::Renderer, "OpenGL implementation doesn't support shaders from binary.");
                break;
            case RendererAPIType::Vulkan:
                //return std::make_shared<VulkanShader>(graphicsContext, vertBin, fragBin);
                break;
        }
        
        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(RendererAPIType api, const std::shared_ptr<GraphicsContext>& graphicsContext, const std::vector<char>& vertBin, const std::vector<char>& fragBin)
    {
        switch (api)
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Shader for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                PXL_LOG_ERROR(LogArea::Renderer, "OpenGL implementation doesn't support shaders from binary.");
                break;
            case RendererAPIType::Vulkan:
                return std::make_shared<VulkanShader>(dynamic_pointer_cast<VulkanContext>(graphicsContext)->GetDevice(), vertBin, fragBin);
        }
        
        return nullptr;
    }

    std::shared_ptr<VulkanShader> Shader::Create(VkDevice device, const std::vector<char>& vertBin, const std::vector<char>& fragBin)
    {
        return std::make_shared<VulkanShader>(device, vertBin, fragBin);
    }
}
