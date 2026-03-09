#include "GraphicsAPI.h"

#include "Vulkan/VulkanAPI.h"

namespace pxl
{
    std::unique_ptr<GraphicsAPI> GraphicsAPI::Create(GraphicsAPIType type)
    {
        switch (type)
        {
            case GraphicsAPIType::Vulkan: return std::make_unique<VulkanAPI>();
            default:                      return nullptr;
        }
    }
}