#pragma once

#include "Core/Image.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace pxl
{
    class FileSystem
    {
    public:
        /// @brief Load an image from disk.
        /// @param path The path of the file. Always include the file format at the end (eg .png).
        /// @param flipVertical Whether to flip the image vertically on load.
        /// @return Image struct with all the loaded data.
        /// @note (For API developer) This function must not be named 'LoadImage' because stupid windows header defines it as something else
        static std::shared_ptr<Image> LoadImageFile(const std::filesystem::path& path, bool flipVertical = false);

        static std::string LoadGLSL(const std::filesystem::path& path);
        static std::vector<uint32_t> LoadSPIRV(const std::filesystem::path& path);
        static std::vector<std::shared_ptr<Mesh>> LoadModel(const std::filesystem::path& path);

        // Path may include directories but for the image to write the directory must already exist.
        static bool WriteImageToFile(const std::filesystem::path& path, const std::shared_ptr<Image>& image, ImageFileFormat fileFormat, bool flipVertical = false);

        // Set the compression level for writing PNG images (higher = more compression). Default is 8.
        static void SetPNGCompressionLevel(int32_t level);

        // Set the quality level for writing JPEG images (must be from 1-100). Higher quality looks better but results in a larger image. Default is 50.
        static void SetJPEGQuality(int32_t qualityLevel) { s_JPEGQuality = qualityLevel; }

    private:
        static inline int32_t s_JPEGQuality = 50; // Valid values are between 1 - 100
    };
}