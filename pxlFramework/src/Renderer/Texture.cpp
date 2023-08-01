#include "Texture.h"
#include "Renderer.h"

#include "OpenGL/OpenGLTexture.h"

namespace pxl
{
    std::shared_ptr<Texture> Texture::Create(unsigned char* imageBuffer, glm::vec2 imageSize, int channels)
    {
        switch (Renderer::GetRendererAPIType())
        {
            case RendererAPIType::None:
                Logger::LogError("Can't create Texture for no renderer api.");
                break;
            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLTexture>(imageBuffer, imageSize, channels);
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't create Texture for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }
}