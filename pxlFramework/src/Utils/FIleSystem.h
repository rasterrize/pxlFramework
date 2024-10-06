#pragma once

#include "Renderer/Image.h"
#include "Renderer/RendererData.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
//#include "Audio/AudioTrack.h"

namespace pxl
{
    class FileSystem
    {
    public:
        // NOTE: Must not be 'LoadImage' because stupid windows header defines it as something else
        static Image LoadImageFile(const std::filesystem::path& path, bool flipOnLoad);

        static std::shared_ptr<Texture> LoadTextureFromImage(const std::filesystem::path& path);
        static std::string LoadGLSL(const std::filesystem::path& path);
        static std::vector<char> LoadSPIRV(const std::filesystem::path& path);
        static std::vector<std::shared_ptr<Mesh>> LoadModel(const std::filesystem::path& path);
        //static std::shared_ptr<AudioTrack> LoadAudioTrack(const std::string& filePath);
    };
}