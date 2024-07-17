#include "Texture.h"

#include "Renderer.h"
#include "OpenGL/OpenGLTexture.h"

namespace pxl
{
    std::shared_ptr<Texture> Texture::Create(const Image& image)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLTexture>(image);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }
}