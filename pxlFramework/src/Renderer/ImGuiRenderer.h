#pragma once

#include "GPUResource.h"

namespace pxl
{
    class GraphicsDevice;

    /// @brief Base class of an API-specific representation of an ImGui implementation
    class ImGuiRenderer : public GPUResource
    {
    public:
        virtual void Free() override = 0;

        virtual void NewFrame() = 0;

        virtual void Render(const std::unique_ptr<GraphicsDevice>& device) = 0;
    };
}