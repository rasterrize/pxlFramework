#include "FileSystem.h"

#include <stb_image.h>
#include <stb_image_write.h>

#include <fstream>
//#include <bass.h>

#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <assimp/Importer.hpp> // C++ importer interface

namespace pxl
{
    std::shared_ptr<Image> FileSystem::LoadImageFile(const std::filesystem::path& path, bool flipVertical)
    {
        // NOTE: The renderer expects things to have their y values start at the bottom, but images are stored from top to bottom, so we automatically flip it unless specified not to.
        stbi_set_flip_vertically_on_load(!flipVertical);

        const int desiredChannels = 4;

        int width, height, channels = 0;
        unsigned char* bytes = stbi_load(path.string().c_str(), &width, &height, &channels, desiredChannels);
        uint64_t numOfBytes = width * height * desiredChannels;

        if (!bytes)
        {
            PXL_LOG_ERROR(LogArea::FileSystem, "Failed to load image: '{}'", path.string());
            return nullptr;
        }

        PXL_LOG_INFO(LogArea::FileSystem, "Loaded image: '{}'", path.string());

        std::vector<uint8_t> buffer;
        buffer.insert(buffer.end(), bytes, bytes + numOfBytes);
        ImageMetadata metadata = { Size2D(width, height), Utils::ToImageFormat(desiredChannels) };
        auto image = std::make_shared<Image>(buffer, metadata);

        PXL_ASSERT(buffer.size() == numOfBytes);

        stbi_image_free(bytes);

        return image;
    }

    std::string FileSystem::LoadGLSL(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            PXL_LOG_ERROR(LogArea::FileSystem, "Failed to load shader from path because the shader path doesn't exist '{}'", path.string());
            return std::string();
        }

        // TODO: rewrite
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

    std::vector<uint32_t> FileSystem::LoadSPIRV(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary); // the 'ate' means read from the end of the file

        if (!file.is_open())
            throw std::runtime_error("Failed to open SPIR-V file");

        // Because we are reading the file from the end, we can tell what size our buffer should be
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0); // return back to the start of the file
        file.read(buffer.data(), fileSize);

        std::vector<uint32_t> code;
        for (size_t i = 0; i < buffer.size(); i += 4)
        {
            uint32_t num;
            memcpy(&num, &buffer[i], 4);

            code.push_back(num);
        }

        return code;
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
                    0);
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

        PXL_LOG_INFO(LogArea::FileSystem, "Loaded model '{}'", path.string());

        return meshes;
    }

    bool FileSystem::WriteImageToFile(const std::filesystem::path& path, const std::shared_ptr<Image>& image, ImageFileFormat fileFormat, bool flipVertical)
    {
        stbi_flip_vertically_on_write(!flipVertical);

        auto size = image->Metadata.Size;
        auto fileNameString = path.string();
        int32_t channels = 0;

        switch (image->Metadata.Format)
        {
            case ImageFormat::RGB8:
                channels = 3;
                break;
            case ImageFormat::RGBA8:
                channels = 4;
                break;
        }

        // TODO: handle trying to write jpg as png (channels don't match)

        switch (fileFormat)
        {
            case ImageFileFormat::PNG:
                return stbi_write_png(fileNameString.c_str(), size.Width, size.Height, channels, image->Buffer.data(), 0);
            case ImageFileFormat::JPG:
                return stbi_write_jpg(fileNameString.c_str(), size.Width, size.Height, channels, image->Buffer.data(), s_JPEGQuality);
            case ImageFileFormat::BMP:
                return stbi_write_bmp(fileNameString.c_str(), size.Width, size.Height, channels, image->Buffer.data());
            default:
                return false;
        }
    }

    void FileSystem::SetPNGCompressionLevel(int32_t level)
    {
        stbi_write_png_compression_level = level;
    }
}