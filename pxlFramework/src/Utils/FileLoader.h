#pragma once

#include <filesystem>

#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/RendererData.h"

#include "../Audio/AudioTrack.h"

namespace pxl
{
    class FileLoader
    {
    public:
        static std::shared_ptr<Texture2D> LoadTextureFromImage(const std::string& filePath);
        static std::shared_ptr<Shader> LoadShader(const std::filesystem::path& path);
        static std::shared_ptr<Mesh> LoadOBJ(const std::string& filePath);
        static std::shared_ptr<AudioTrack> LoadAudioTrack(const std::string& filePath);
    };
}