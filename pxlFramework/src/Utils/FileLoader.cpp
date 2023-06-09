#include "FileLoader.h"
#include <stb_image.h>

namespace pxl
{
    std::shared_ptr<Texture> FileLoader::LoadTexture(const std::string& filePath) // could be load image as well ??
    {
        int width, height, channels;
        unsigned char* bytes = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        //stbi_set_flip_vertically_on_load(1);

        std::shared_ptr<Texture> texture = Texture::Create(bytes, glm::vec2(width, height), channels); // should this be using glm?
        stbi_image_free(bytes);

        return texture;
    }

    std::shared_ptr<Shader> FileLoader::LoadShader(const std::filesystem::path& path)
    {
        return nullptr;
    }

    std::shared_ptr<Mesh> FileLoader::LoadOBJ(const std::string& filePath)
    {
        // std::ifstream file(filePath);

        // std::string line;
        // std::getline(file, line);

        // Logger::LogInfo(line);
        return nullptr;
    }    
}