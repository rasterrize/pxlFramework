#include "GraphicsContext.h"

#include "OpenGL/OpenGLContext.h"
#include "Vulkan/VulkanContext.h"

namespace pxl
{
    std::shared_ptr<GraphicsContext> GraphicsContext::Create(RendererAPIType api, const std::shared_ptr<Window>& window)
    {
        switch (api)
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Graphics Context for no renderer api.");
                return nullptr;
            case RendererAPIType::OpenGL:   return std::make_shared<OpenGLGraphicsContext>(window);
            case RendererAPIType::Vulkan:   return std::make_shared<VulkanGraphicsContext>(window);
        }

        return nullptr;
    }
}