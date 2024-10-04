#include "FileSystem.h"

#include <stb_image.h>

#include <fstream>
//#include <bass.h>

#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <assimp/Importer.hpp> // C++ importer interface

namespace pxl
{
    Image FileSystem::LoadImageFile(const std::filesystem::path& path, bool flipOnLoad)
    {
        // stb image loads images from bottom to top by default so we flip them on load
        stbi_set_flip_vertically_on_load(flipOnLoad);

        int width, height, channels;
        unsigned char* bytes = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        if (!bytes)
        {
            PXL_LOG_ERROR(LogArea::Other, "Failed to load image: '{}'", path.string());
            return Image();
        }

        PXL_LOG_INFO(LogArea::Other, "Loaded image: '{}'", path.string());

        Image image;
        image.Buffer = bytes;
        image.Metadata.Size = { (uint32_t)width, (uint32_t)height };
        image.Metadata.Format = ImageFormat::Undefined;

        switch (channels)
        {
            case 3: image.Metadata.Format = ImageFormat::RGB8; break;
            case 4: image.Metadata.Format = ImageFormat::RGBA8; break;
        }

        return image;
    }

    std::shared_ptr<Texture> FileSystem::LoadTextureFromImage(const std::filesystem::path& path)
    {
        auto image = LoadImageFile(path, true);

        std::shared_ptr<Texture> texture = Texture::Create(image);

        image.Free();

        return texture;
    }

    std::string FileSystem::LoadGLSL(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            PXL_LOG_ERROR(LogArea::Other, "Failed to load shader from path because the shader path doesn't exist '{}'", path.string());
            return std::string();
        }

        std::ifstream file(path, std::ios::ate | std::ios::binary); // the 'ate' means read from the end of the file

        if (!file.is_open())
            throw std::runtime_error("Failed to open shader file");

        std::string src;
        std::ifstream in(path, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                src.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&src[0], size);
            }
        }

        return src;
    }

    std::vector<char> FileSystem::LoadSPIRV(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary); // the 'ate' means read from the end of the file

        if (!file.is_open())
            throw std::runtime_error("Failed to open shader file");

        size_t fileSize = (size_t)file.tellg(); // because we are reading the file from the end, we can tell what size our buffer should be
        std::vector<char> buffer(fileSize);

        file.seekg(0); // return back to the start of the file
        file.read(buffer.data(), fileSize);

        //file.close(); // RAII closes it

        return buffer;
    }

    std::shared_ptr<Mesh> FileSystem::LoadOBJ(const std::filesystem::path& filePath)
    {
        Assimp::Importer importer;

        // Load file from disk
        const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        if (!scene)
        {
            PXL_LOG_WARN(LogArea::FileSystem, "Failed to load OBJ file from path '{}'", filePath.string());
            return nullptr;
        }

        auto mesh = std::make_shared<Mesh>();

        // Go through all the meshes in the file
        for (uint32_t m = 0; m < scene->mNumMeshes; m++)
        {
            // Go through all vertices in the current mesh
            for (uint32_t v = 0; v < scene->mMeshes[m]->mNumVertices; v++)
            {
                // Positions
                float x = scene->mMeshes[m]->mVertices[v].x;
                float y = scene->mMeshes[m]->mVertices[v].y;
                float z = scene->mMeshes[m]->mVertices[v].z;

                // Colours
                auto vertexColour = scene->mMeshes[m]->mColors[0];
                mesh->Vertices.push_back({{ x, y, z }, { vertexColour->r, vertexColour->g, vertexColour->b, vertexColour->a }, { 0.0f, 0.0f }});
            }

            // Go through all the faces and indices of the current mesh
            for (uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
            {
                for (uint32_t i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; i++)
                {
                    mesh->Indices.push_back(scene->mMeshes[m]->mFaces[f].mIndices[i]);
                }
            }
        }

        return mesh;
    }

    std::vector<std::shared_ptr<Mesh>> FileSystem::LoadFBX(const std::filesystem::path& filePath)
    {
        Assimp::Importer importer;

        auto meshes = std::vector<std::shared_ptr<Mesh>>();

        auto mesh = std::make_shared<Mesh>();

        const aiScene* scene = importer.ReadFile(filePath.string().c_str(), aiProcess_Triangulate);

        if (!scene)
            return std::vector<std::shared_ptr<Mesh>>();

        for (uint32_t i = 0; i < scene->mNumMeshes; i++)
        {
            for (uint32_t n = 0; n < scene->mMeshes[i]->mNumVertices; n++)
            {
                [[maybe_unused]] float x = scene->mMeshes[i]->mVertices[n].x;
                [[maybe_unused]] float y = scene->mMeshes[i]->mVertices[n].y;
                [[maybe_unused]] float z = scene->mMeshes[i]->mVertices[n].z;
            }
        }

        return std::vector<std::shared_ptr<Mesh>>();
    }

    // std::shared_ptr<AudioTrack> FileSystem::LoadAudioTrack(const std::string& filePath)
    // {
    //     HSTREAM stream = BASS_StreamCreateFile(FALSE, filePath.c_str(), 0, 0, BASS_SAMPLE_FLOAT);

    //     if (stream)
    //         Logger::LogInfo("Loaded audio: '" + filePath + "'");
    //     else
    //         Logger::LogError("Failed to load audio: '" + filePath + "'");

    //     return std::make_shared<AudioTrack>(stream);
    // }
}