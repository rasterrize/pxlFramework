#include "GraphicsInstance.h"

#include "Vulkan/VulkanGraphicsInstance.h"

namespace pxl
{
    std::unique_ptr<GraphicsInstance> GraphicsInstance::Create(GraphicsAPI type)
    {
        switch (type)
        {
            case GraphicsAPI::Vulkan: return std::make_unique<VulkanGraphicsInstance>();
            default:                  return nullptr;
        }
    }
}