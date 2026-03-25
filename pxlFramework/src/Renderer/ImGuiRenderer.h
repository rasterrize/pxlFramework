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
        ImGuiRenderer(const ImGuiSpecs& specs)
            : m_Specs(specs)
        {
        }

        virtual void Free() override = 0;

        virtual void NewFrame() = 0;

        virtual void Render(const std::unique_ptr<GraphicsDevice>& device) = 0;

    protected:
        ImGuiSpecs m_Specs = {};
    };
}