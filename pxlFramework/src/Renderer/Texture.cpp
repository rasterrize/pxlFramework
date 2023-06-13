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
            case RendererAPIType::DirectX11:
                Logger::LogError("Can't create Texture for DirectX11 renderer api.");
                break;
            case RendererAPIType::DirectX12:
                Logger::LogError("Can't create Texture for DirectX12 renderer api.");
                break;
        }

        return nullptr;
    }
}