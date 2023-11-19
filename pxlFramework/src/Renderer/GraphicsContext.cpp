#include "GraphicsContext.h"

#include "OpenGL/OpenGLContext.h"

#include "../Core/Window.h"

namespace pxl
{
    std::shared_ptr<GraphicsContext> GraphicsContext::Create(RendererAPIType api, const std::shared_ptr<Window>& window)
    {
        switch (api)
        {
            case RendererAPIType::None:
                Logger::LogError("Can't create Graphics Context for no renderer api.");
                return nullptr;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLContext>(window->GetNativeWindow());
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't create Graphics Context for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }
}