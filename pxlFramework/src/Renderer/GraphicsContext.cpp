#include "GraphicsContext.h"

#include "OpenGL/OpenGLContext.h"

#include <GLFW/glfw3native.h>

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
            case RendererAPIType::DirectX11:
                //return std::make_shared<Direct3D11Context>(glfwGetWin32Window(windowHandle));
                Logger::LogError("Can't create Graphics Context for DirectX11 renderer api.");
                break;
            case RendererAPIType::DirectX12:
                Logger::LogError("Can't create Graphics Context for DirectX12 renderer api.");
                break;
        }

        return nullptr;
    }
}