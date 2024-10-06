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

    std::vector<std::shared_ptr<Mesh>> FileSystem::LoadModel(const std::filesystem::path& path)
    {
        Assimp::Importer importer;

        // Load file from disk
        const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        if (!scene)
        {
            PXL_LOG_WARN(LogArea::FileSystem, "Failed to load model file from path '{}'", path.string());
            return std::vector<std::shared_ptr<Mesh>>();
        }

        std::vector<std::shared_ptr<Mesh>> meshes(scene->mNumMeshes);

        // Go through all the meshes in the file
        for (uint32_t m = 0; m < scene->mNumMeshes; m++)
        {
            auto mesh = std::make_shared<Mesh>(scene->mMeshes[m]->mNumVertices, scene->mMeshes[m]->mFaces->mNumIndices);

            // Go through all vertices in the current mesh
            for (uint32_t v = 0; v < scene->mMeshes[m]->mNumVertices; v++)
            {
                aiVector3D assVertex = scene->mMeshes[m]->mVertices[v];
                aiColor4D* assColor = scene->mMeshes[m]->mColors[0];
                glm::vec4 vertexColour = glm::vec4(1.0f);

                if (assColor)
                    vertexColour = glm::vec4(assColor->r, assColor->g, assColor->b, assColor->a);

                mesh->Vertices.emplace_back(
                    glm::vec3(assVertex.x, assVertex.y, assVertex.z),
                    vertexColour,
                    glm::vec2(0.0f),
                    -1.0f);
            }

            // Go through all the faces and indices of the current mesh
            for (uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
            {
                for (uint32_t i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; i++)
                {
                    mesh->Indices.emplace_back(scene->mMeshes[m]->mFaces[f].mIndices[i]);
                }
            }

            meshes[m] = mesh;
        }

        return meshes;
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