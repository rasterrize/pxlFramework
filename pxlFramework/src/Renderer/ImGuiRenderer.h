#pragma once

#include "Core/Window.h"
#include "GPUResource.h"

namespace pxl
{
    class GraphicsDevice;

    struct ImGuiSpecs
    {
        std::shared_ptr<Window> Window = nullptr;
    };

    /// @brief Base class of an API-specific representation of an ImGui implementation.
    class ImGuiRenderer : public GPUResource
    {
    public:
        virtual void NewFrame() = 0;

        virtual void Render(const std::unique_ptr<GraphicsDevice>& device, uint32_t frameIndex) = 0;
    };
}