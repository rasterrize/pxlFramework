#pragma once

#include <glm/vec4.hpp>

#include "Core/Window.h"
#include "GraphicsAPIType.h"
#include "Texture.h"

namespace pxl
{
    struct ShaderFile;

    enum class FramerateMode
    {
        Unlimited,
        Custom,
        AdaptiveSync,
    };

    namespace RendererConstants
    {
        static const std::filesystem::path FrameworkShaderDirectory = PXL_RESOURCE_DIRECTORY "/shaders";

        static const uint32_t VerticesPerQuad = 4;
        static const uint32_t IndicesPerQuad = 6;

        static const uint32_t MaxFramesInFlight = 2;

        static const bool DefaultVerticalSync = true;
        static const bool DefaultAllowTearing = false;
        static const bool DefaultTripleBuffering = true;
        static const GraphicsAPIType DefaultGraphicsAPI = GraphicsAPIType::Vulkan;
        static const bool DefaultUseShaderCache = true;

        static const FramerateMode DefaultFramerateMode = FramerateMode::Unlimited;
        static const uint32_t DefaultCustomFramerateLimit = 60;
        static const uint32_t DefaultUnfocusedFramerateLimit = 60;
    }

    struct RendererConfig
    {
        /// @brief The window to associate the renderer to. Must be a valid window pointer.
        std::shared_ptr<Window> Window;

        /// @brief The backend graphics API to use for rendering
        GraphicsAPIType APIType = RendererConstants::DefaultGraphicsAPI;

        bool TripleBuffering = RendererConstants::DefaultTripleBuffering;

        /// @brief Sync frame presentation with the refresh interval of the display.
        bool VerticalSync = RendererConstants::DefaultVerticalSync;

        bool AllowTearing = RendererConstants::DefaultAllowTearing;

        FramerateMode FramerateMode = RendererConstants::DefaultFramerateMode;

        uint32_t CustomFramerateLimit = RendererConstants::DefaultCustomFramerateLimit;

        uint32_t UnfocusedFramerateLimit = RendererConstants::DefaultUnfocusedFramerateLimit;

        /// @brief A colour to clear the initial screen with.
        glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };

        uint32_t VerticesPerBatch = 1000;

        /// @brief Cache compiled shaders to disk.
        bool UseShaderCache = RendererConstants::DefaultUseShaderCache;

        std::filesystem::path ShaderCacheDirectory = ".shadercache";

        /// @brief A list of shader files to be added to the shader manager.
        std::vector<ShaderFile> UserShadersToCompile;

        /// @brief Automatically initialize ImGui after renderer is initialized.
        bool InitImGui = false;

        float AnisotropicFilteringLevel = 1.0f;
    };

    namespace Utils
    {
        inline std::string ToString(FramerateMode mode)
        {
            switch (mode)
            {
                case FramerateMode::Unlimited:    return "Unlimited";
                case FramerateMode::Custom:       return "Custom";
                case FramerateMode::AdaptiveSync: return "Adaptive Sync";
                default:                          return "Unknown";
            }
        }

        inline FramerateMode ToFramerateMode(std::string string)
        {
            const std::unordered_map<std::string, FramerateMode> stringToFramerateMode = {
                {    Utils::ToString(FramerateMode::Unlimited),    FramerateMode::Unlimited },
                {       Utils::ToString(FramerateMode::Custom),       FramerateMode::Custom },
                { Utils::ToString(FramerateMode::AdaptiveSync), FramerateMode::AdaptiveSync },
            };

            try
            {
                return stringToFramerateMode.at(string);
            }
            catch (std::out_of_range e)
            {
                return FramerateMode::Unlimited;
            }
        }
    }
}