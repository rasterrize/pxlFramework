#pragma once

#include "GraphicsContext.h"
#include "GraphicsDevice.h"
#include "Core/Window.h"
#include "GraphicsAPIType.h"

namespace pxl
{
    /// @brief Represents the base level functionality of a graphics API
    class GraphicsAPI
    {
    public:
        virtual ~GraphicsAPI() = default;

        virtual std::unique_ptr<GraphicsContext> CreateGraphicsContext() = 0;

        virtual std::unique_ptr<GraphicsDevice> CreateGraphicsDevice(const GraphicsDeviceSpecs& specs) = 0;
    
    public:
        static std::unique_ptr<GraphicsAPI> Create(GraphicsAPIType type);
    };
}