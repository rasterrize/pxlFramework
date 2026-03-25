#pragma once

#include <glm/vec4.hpp>

#include "Core/Window.h"
#include "GraphicsAPIType.h"

namespace pxl
{
    struct RendererConfig
    {
        /// @brief The window to associate the renderer to. Must be a valid window pointer.
        std::shared_ptr<Window> Window = nullptr;
        GraphicsAPIType APIType = GraphicsAPIType::Vulkan;
        bool TripleBuffering = true;
        bool VerticalSync = true;
        glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 1.0f };
        uint32_t VerticesPerBatch = 1000;
    };
}