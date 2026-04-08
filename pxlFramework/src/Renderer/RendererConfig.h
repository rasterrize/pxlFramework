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

        /// @brief The backend graphics API to use for rendering
        GraphicsAPIType APIType = GraphicsAPIType::Vulkan;

        bool TripleBuffering = true;

        /// @brief Sync frame presentation with the refresh interval of the display.
        bool VerticalSync = true;

        /// @brief A colour to clear the initial screen with.
        glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };

        uint32_t VerticesPerBatch = 1000;

        /// @brief Cache compiled shaders to disk.
        bool UseShaderCache = true;

        std::filesystem::path ShaderCacheDirectory = ".shadercache";

        /// @brief A list of shader files to be added to the shader manager.
        std::vector<ShaderFile> UserShadersToCompile;

        /// @brief Automatically initialize ImGui after renderer is initialized.
        bool InitImGui = false;
    };
}