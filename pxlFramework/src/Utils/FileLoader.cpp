#include "FileLoader.h"

#include <stb_image.h>
#include <fstream>
#include <bass.h>

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
            Logger::LogInfo("Loaded texture: '" + filePath + "'");
        }
        else
        {
            Logger::LogError("Failed to load texture: '" + filePath + "'");
            return nullptr;
        }

        std::shared_ptr<Texture> texture = Texture::Create(bytes, { width, height }, channels); // should glm be used here?

        // stb image requires we manually free the loaded image from memory
        stbi_image_free(bytes);

        return texture;
    }

    std::shared_ptr<Shader> FileLoader::LoadShader(const std::filesystem::path& path)
    {
        const std::string vertID = "#type vertex";
        const std::string fragID = "#type fragment";
    
        if (!std::filesystem::exists(path))
        {
            Logger::LogError("Failed to load shader from path because the file doesn't exist '" + path.string() + "'");
            return nullptr;
        }

        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (file)
        {
            Logger::LogInfo("Loaded shader: '" + path.string() + "'");
        }
        else
        {
            Logger::LogInfo("Failed to load shader: '" + path.string() + "'");
        }
    

        file.close();

        //std::shared_ptr<Shader> shader = Shader::Create
    
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
    std::shared_ptr<AudioTrack> FileLoader::LoadAudioTrack(const std::string& filePath)
    {
        HSTREAM stream = BASS_StreamCreateFile(FALSE, filePath.c_str(), 0, 0, BASS_SAMPLE_FLOAT);

        if (stream)
            Logger::LogInfo("Audio loaded successfully: '" + filePath + "'");
        else
            Logger::LogError("Audio failed to load: '" + filePath + "'");

        return std::make_shared<AudioTrack>(stream);
    }
}