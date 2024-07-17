#pragma once

#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/RendererData.h"

//#include "../Audio/AudioTrack.h"

namespace pxl
{
    class FileLoader
    {
    public:
        static std::shared_ptr<Texture> LoadTextureFromImage(const std::string& filePath);
        static std::string LoadGLSL(const std::filesystem::path& path);
        static std::vector<char> LoadSPIRV(const std::filesystem::path& path);
        static std::shared_ptr<Mesh> LoadOBJ(const std::string& filePath);
        static std::vector<std::shared_ptr<Mesh>> LoadFBX(const std::string& filePath);
        //static std::shared_ptr<AudioTrack> LoadAudioTrack(const std::string& filePath);
    };
}