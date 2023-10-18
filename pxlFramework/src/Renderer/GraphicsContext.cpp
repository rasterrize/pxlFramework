#include "GraphicsContext.h"

#include "OpenGL/OpenGLContext.h"

namespace pxl
{
    std::shared_ptr<GraphicsContext> GraphicsContext::Create(RendererAPIType api, GLFWwindow* windowHandle)
    {
        switch (api)
        {
            case RendererAPIType::None:
                Logger::LogError("Can't create Graphics Context for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLContext>(windowHandle);
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't create Graphics Context for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }
}