#include "RendererAPI.h"

#include "OpenGL/OpenGLRenderer.h"
#include "Vulkan/VulkanRenderer.h"

namespace pxl
{
    std::unique_ptr<RendererAPI> RendererAPI::Create(RendererAPIType api, const std::shared_ptr<Window>& window)
    {
        switch (api)
        {
            case RendererAPIType::None:   break;
            case RendererAPIType::OpenGL: return std::make_unique<OpenGLRenderer>();
            case RendererAPIType::Vulkan: return std::make_unique<VulkanRenderer>(static_pointer_cast<VulkanGraphicsContext>(window->GetGraphicsContext()));
        }

        PXL_LOG_ERROR(LogArea::Renderer, "Failed to create RendererAPI");

        return nullptr;
    }
}