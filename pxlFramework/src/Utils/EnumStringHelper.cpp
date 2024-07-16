#include "EnumStringHelper.h"

namespace pxl
{
    std::string EnumStringHelper::RendererAPITypeToString(RendererAPIType type)
    {
        switch (type)
        {
            case RendererAPIType::None:   return "None";
            case RendererAPIType::OpenGL: return "OpenGL";
            case RendererAPIType::Vulkan: return "Vulkan";
        }

        return std::string();
    }
}