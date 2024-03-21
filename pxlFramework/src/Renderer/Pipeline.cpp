#include "Pipeline.h"

#include "Vulkan/VulkanPipeline.h"
#include "OpenGL/OpenGLPipeline.h"
#include "Renderer.h"

namespace pxl
{
    std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const std::shared_ptr<Shader>& shader, const BufferLayout& vertexLayout)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLGraphicsPipeline>(shader);
            case RendererAPIType::Vulkan:
                auto device = dynamic_pointer_cast<VulkanDevice>(Renderer::GetCurrentDevice());
                if (!device)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline, failed to retrieve VulkanDevice from renderer");
                    break;
                }

                auto context = std::dynamic_pointer_cast<VulkanContext>(Renderer::GetWindowHandle()->GetGraphicsContext());
                if (!context)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline, failed to retrieve Vulkan GraphicsContext from renderer");
                    break;
                }

                auto vulkanShader = std::dynamic_pointer_cast<VulkanShader>(shader);
                if (!shader)
                {
                    PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Pipeline, failed to retrieve VulkanShader from renderer");
                    break;
                }

                return std::make_shared<VulkanGraphicsPipeline>(context, device, vulkanShader, context->GetDefaultRenderPass(), vertexLayout);
        }

        return nullptr;
    }
}