#include "Pipeline.h"

#include "Renderer.h"
#include "Vulkan/VulkanPipeline.h"
#include "OpenGL/OpenGLPipeline.h"
#include "Vulkan/VulkanContext.h"


namespace pxl
{
    std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const std::shared_ptr<Shader>& shader)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline for no renderer api.");
                return nullptr;
            case RendererAPIType::OpenGL: return std::make_shared<OpenGLGraphicsPipeline>(shader);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline for Vulkan using a single shader.")
                return nullptr;
        }

        return nullptr;
    }

    std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders, const BufferLayout& vertexLayout, const UniformLayout& uniformLayout)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline for no renderer api.");
                return nullptr;
            case RendererAPIType::OpenGL:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline for OpenGL using multiple shaders.");
                return nullptr;
            case RendererAPIType::Vulkan:
                auto context = std::static_pointer_cast<VulkanGraphicsContext>(Renderer::GetGraphicsContext());

                return std::make_shared<VulkanGraphicsPipeline>(shaders, context->GetDefaultRenderPass(), vertexLayout, uniformLayout);
        }

        return nullptr;
    }
}