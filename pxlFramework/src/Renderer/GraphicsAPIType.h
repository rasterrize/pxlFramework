#pragma once

namespace pxl
{
    enum class GraphicsAPIType
    {
        Vulkan,
    };

    namespace Utils
    {
        inline std::string ToString(GraphicsAPIType type)
        {
            switch (type)
            {
                case GraphicsAPIType::Vulkan: return "Vulkan";
                default:                      return "Unknown";
            }
        }
    }
}