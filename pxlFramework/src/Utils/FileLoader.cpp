#include "FileLoader.h"

#include <stb_image.h>
#include <fstream>
//#include <bass.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace pxl
{
    std::shared_ptr<Texture2D> FileLoader::LoadTextureFromImage(const std::string& filePath)
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

        std::shared_ptr<Texture2D> texture = Texture2D::Create(bytes, { width, height }, channels); // should glm be used here?

        // stb image requires we manually free the loaded image from memory
        stbi_image_free(bytes);

        return texture;
    }

    std::shared_ptr<Shader> FileLoader::LoadGLSLShader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath)
    {    
        if (!std::filesystem::exists(vertPath))
        {
            PXL_LOG_ERROR(LogArea::Other, "Failed to load shader from path because the vertex shader path doesn't exist '{}'", vertPath.string());
            return nullptr;
        }

        if (!std::filesystem::exists(fragPath))
        {
            PXL_LOG_ERROR(LogArea::Other, "Failed to load shader from path because the fragment shader path doesn't exist '{}'", fragPath.string());
            return nullptr;
        }

        std::ifstream file(vertPath, std::ios::ate | std::ios::binary); // the 'ate' means read from the end of the file

        if (!file.is_open())
            throw std::runtime_error("Failed to open shader file");
        
        std::string vertSrc;
		std::ifstream in(vertPath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				vertSrc.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&vertSrc[0], size);
			}
		}

        std::string fragSrc;
		std::ifstream in2(fragPath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in2)
		{
			in2.seekg(0, std::ios::end);
			size_t size = in2.tellg();
			if (size != -1)
			{
				fragSrc.resize(size);
				in2.seekg(0, std::ios::beg);
				in2.read(&fragSrc[0], size);
			}
		}

        return Shader::Create(vertSrc, fragSrc);
    }

    std::vector<char> FileLoader::LoadSPIRV(const std::filesystem::path& path)
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

    std::shared_ptr<Mesh> FileLoader::LoadOBJ(const std::string& filePath)
    {
        Assimp::Importer importer;

        // Load file from disk
        const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        if (!scene)
            return nullptr;

        auto mesh = std::make_shared<Mesh>();

        // Go through all the meshes in the file
        for (int m = 0; m < scene->mNumMeshes; m++)
        {
            // Go through all vertices in the current mesh
            for (int v = 0; v < scene->mMeshes[m]->mNumVertices; v++)
            {
                float x = scene->mMeshes[m]->mVertices[v].x;
                float y = scene->mMeshes[m]->mVertices[v].y;
                float z = scene->mMeshes[m]->mVertices[v].z;

                mesh->Vertices.push_back({{x, y, z}, glm::vec4(1.0f), { 0.0f, 0.0f }});
            }

            // Go through all the faces and indices of the current mesh
            for (int f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
            {
                for (int i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; i++)
                {
                    mesh->Indices.push_back(scene->mMeshes[m]->mFaces[f].mIndices[i]);
                }
            }
        }

        return mesh;
    }

    std::vector<std::shared_ptr<Mesh>> FileLoader::LoadFBX(const std::string& filePath)
    {
        Assimp::Importer importer;

        auto meshes = std::vector<std::shared_ptr<Mesh>>();

        auto mesh = std::make_shared<Mesh>();

        const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate);

        if (!scene)
            return std::vector<std::shared_ptr<Mesh>>();

        for (int i = 0; i < scene->mNumMeshes; i++)
        {
            for (int n = 0; n < scene->mMeshes[i]->mNumVertices; n++)
            {
                float x = scene->mMeshes[i]->mVertices[n].x;
                float y = scene->mMeshes[i]->mVertices[n].y;
                float z = scene->mMeshes[i]->mVertices[n].z;
            }
        }

        return std::vector<std::shared_ptr<Mesh>>();
    }

    // std::shared_ptr<AudioTrack> FileLoader::LoadAudioTrack(const std::string& filePath)
    // {
    //     HSTREAM stream = BASS_StreamCreateFile(FALSE, filePath.c_str(), 0, 0, BASS_SAMPLE_FLOAT);

    //     if (stream)
    //         Logger::LogInfo("Loaded audio: '" + filePath + "'");
    //     else
    //         Logger::LogError("Failed to load audio: '" + filePath + "'");

    //     return std::make_shared<AudioTrack>(stream);
    // }
}