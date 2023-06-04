#include "TextureLoader.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/OpenGL/OpenGLTexture.h"

#include <stb_image.h>
#include <glm/vec2.hpp>

namespace pxl
{
    std::shared_ptr<Texture> TextureLoader::Load(const std::string& filePath)
    {
        int width, height, channels;
        unsigned char* bytes = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        //stbi_set_flip_vertically_on_load(1);

        std::shared_ptr<Texture> texture;

        switch (Renderer::GetRendererAPIType())
        {
            case RendererAPIType::OpenGL:
                texture = std::make_shared<OpenGLTexture>(bytes, glm::vec2(width, height), channels);
                stbi_image_free(bytes);
                return texture;
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't create Vulkan texture because vulkan isn't supported");
                return nullptr;
            case RendererAPIType::DirectX12:
                Logger::LogError("Can't create DirectX12 texture because DirectX12 isn't supported");
                return nullptr;
        }

        return nullptr;
    }
}