#pragma once

namespace pxl
{
    enum class GraphicsAPI
    {
        Vulkan,
    };

    namespace Utils
    {
        inline std::string ToString(GraphicsAPI api)
        {
            switch (api)
            {
                case GraphicsAPI::Vulkan: return "Vulkan";
                default:                  return "Unknown";
            }
        }
    }
}