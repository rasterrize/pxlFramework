#pragma once

#include <glm/vec4.hpp>

#include "Core/Window.h"
#include "GraphicsAPIType.h"

namespace pxl
{
    struct ShaderFile;

    struct RendererConfig
    {
        /// @brief The window to associate the renderer to. Must be a valid window pointer.
        std::shared_ptr<Window> Window = nullptr;
        GraphicsAPIType APIType = GraphicsAPIType::Vulkan;
        bool TripleBuffering = true;
        bool VerticalSync = true;
        glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };
        uint32_t VerticesPerBatch = 1000;

#ifdef PXL_DEBUG
        bool UseShaderCache = false;
#else
        bool UseShaderCache = true;
#endif

        std::filesystem::path ShaderCacheDirectory = ".shadercache";

        /// @brief A list of shader files to be added to the shader manager
        std::vector<ShaderFile> UserShadersToCompile;
    };
}