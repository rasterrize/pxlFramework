#include "Texture.h"

#include "OpenGL/OpenGLTexture.h"
#include "Renderer.h"

namespace pxl
{
    std::shared_ptr<Texture> Texture::Create(const Image& image, const TextureSpecs& specs)
    {
        // If the image is invalid, return a error texture
        if (image.Buffer.empty())
            return CreateErrorTexture(specs);

        if (image.Metadata.Format == ImageFormat::RGB8 && image.Metadata.Size.Width % 2 != 0)
            PXL_LOG_WARN(LogArea::Renderer, "Image supplied for texture creation is RGB only AND its width is not a power of two. This texture may not render correctly!");

        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for no renderer api.");
                break;

            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLTexture>(image, specs);

            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }

    std::shared_ptr<Texture> Texture::Create(const std::shared_ptr<Image>& image, const TextureSpecs& specs)
    {
        // If the image is invalid, return a error texture
        if (!image)
            return CreateErrorTexture(specs);

        if (image->Metadata.Format == ImageFormat::RGB8 && image->Metadata.Size.Width % 2 != 0)
            PXL_LOG_WARN(LogArea::Renderer, "Image supplied for texture creation is RGB only AND its width is not a power of two. This texture may not render correctly!");

        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for no renderer api.");
                break;

            case RendererAPIType::OpenGL:
                return std::make_shared<OpenGLTexture>(image, specs);

            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create Texture for Vulkan renderer api.");
                break;
        }

        return nullptr;
    }

    std::shared_ptr<Texture> Texture::CreateErrorTexture(const TextureSpecs& specs)
    {
        // TODO: precalculate this data from Renderer::Init

        // Manually create error image
        Image image;

        const uint32_t textureSize = 8;

        uint32_t index = 2;

        for (uint32_t y = 0; y < textureSize; y++)
        {
            index++;

            for (uint32_t x = 0; x < textureSize; x++)
            {
                if (index % 2)
                {
                    // Red
                    image.Buffer.emplace_back(255); // R
                    image.Buffer.emplace_back(0);   // G
                    image.Buffer.emplace_back(255); // B
                    image.Buffer.emplace_back(255); // A
                }
                else
                {
                    // Grey
                    image.Buffer.emplace_back(50);  // R
                    image.Buffer.emplace_back(50);  // G
                    image.Buffer.emplace_back(50);  // B
                    image.Buffer.emplace_back(255); // A
                }

                index++;
            }
        }

        image.Metadata = {
            .Size = { textureSize, textureSize },
            .Format = ImageFormat::RGBA8,
        };

        // Create error texture
        switch (Renderer::GetCurrentAPI())
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create error texture for no renderer api");
                return nullptr;

            case RendererAPIType::OpenGL:
                PXL_LOG_WARN(LogArea::Renderer, "Creating error texture");
                return std::make_shared<OpenGLTexture>(image, specs);

            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Renderer, "Can't create error Texture for Vulkan renderer api");
                return nullptr;
            default:
                return nullptr;
        }
    }
}