#include "Texture.h"
#include "Renderer.h"

#include "OpenGL/OpenGLTexture2D.h"

namespace pxl
{
    std::shared_ptr<Texture2D> Texture2D::Create(unsigned char* imageBuffer, const glm::vec2& imageSize, int channels)
    {
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLTexture2D>(imageBuffer, imageSize, channels);
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }
}