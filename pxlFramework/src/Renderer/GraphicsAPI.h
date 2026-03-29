#pragma once

#include "Core/Window.h"
#include "GraphicsAPIType.h"
#include "GraphicsContext.h"
#include "GraphicsDevice.h"

namespace pxl
{
    /// @brief Represents the base level functionality of a graphics API.
    class GraphicsAPI
    {
    public:
        virtual ~GraphicsAPI() = default;

        virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext(const GraphicsContextSpecs& specs) = 0;

        virtual std::unique_ptr<GraphicsDevice> CreateGraphicsDevice(const GraphicsDeviceSpecs& specs) = 0;

        static std::unique_ptr<GraphicsAPI> Create(GraphicsAPIType type);
    };
}