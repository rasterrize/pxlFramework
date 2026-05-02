#include "FileSystem.h"

#include <stb_image.h>
#include <stb_image_write.h>

#include <fstream>

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
            PXL_LOG_ERROR("Failed to load image: '{}'", path.string());
            return nullptr;
        }

        PXL_LOG_INFO("Loaded image: '{}'", path.string());

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
            PXL_LOG_ERROR("Failed to load shader from path because the shader path doesn't exist '{}'", path.string());
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
            int64_t size = in.tellg();
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

    bool FileSystem::WriteImageToFile(const std::filesystem::path& path, const Image& image, ImageFileFormat fileFormat, bool flipVertical)
    {
        stbi_flip_vertically_on_write(!flipVertical);

        auto size = image.Metadata.Size;
        auto fileNameString = path.string();
        int32_t channels = 0;

        switch (image.Metadata.Format)
        {
            case ImageFormat::RGB8:
                channels = 3;
                break;
            case ImageFormat::RGBA8:
                channels = 4;
                break;
            default: channels = 0; break;
        }

        // TODO: handle trying to write jpg as png (channels don't match)

        switch (fileFormat)
        {
            case ImageFileFormat::PNG:
                return stbi_write_png(fileNameString.c_str(), size.Width, size.Height, channels, image.Buffer.data(), 0);
            case ImageFileFormat::JPG:
                return stbi_write_jpg(fileNameString.c_str(), size.Width, size.Height, channels, image.Buffer.data(), s_JPEGQuality);
            case ImageFileFormat::BMP:
                return stbi_write_bmp(fileNameString.c_str(), size.Width, size.Height, channels, image.Buffer.data());
            default:
                return false;
        }
    }

    void FileSystem::SetPNGCompressionLevel(int32_t level)
    {
        stbi_write_png_compression_level = level;
    }
}