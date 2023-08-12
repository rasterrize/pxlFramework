#include "FileLoader.h"

#include <stb_image.h>
#include <fstream>

namespace pxl
{
    std::shared_ptr<Texture> FileLoader::LoadTextureFromImage(const std::string& filePath)
    {
        // stb image loads images from bottom to top I guess
        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        unsigned char* bytes = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        if (bytes)
        {
            Logger::LogInfo("Successfully loaded texture '" + filePath + "'");
        }
        else
        {
            Logger::LogError("Failed to load texture '" + filePath + "'");
            return nullptr;
        }

        std::shared_ptr<Texture> texture = Texture::Create(bytes, glm::vec2(width, height), channels); // should glm be used here?

        // stb image requires we manually free the loaded image from memory
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