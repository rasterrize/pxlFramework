#include "Pipeline.h"

#include "OpenGL/OpenGLPipeline.h"
#include "Renderer.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanPipeline.h"

namespace pxl
{
    std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineSpecs& specs, const std::unordered_map<ShaderStage, std::shared_ptr<Shader>>& shaders)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:   PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline for no renderer api."); return nullptr;
            case RendererAPIType::OpenGL: return std::make_shared<OpenGLGraphicsPipeline>(shaders);
            case RendererAPIType::Vulkan:
                auto context = std::static_pointer_cast<VulkanGraphicsContext>(Renderer::GetGraphicsContext());
                return std::make_shared<VulkanGraphicsPipeline>(specs, shaders, context->GetDefaultRenderPass());
        }

        return nullptr;
    }
}