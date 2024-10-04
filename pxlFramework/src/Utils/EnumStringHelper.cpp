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

        return "Undefined";
    }

    std::string EnumStringHelper::ToString(WindowMode mode)
    {
        switch (mode)
        {
            case WindowMode::Windowed:   return "Windowed";
            case WindowMode::Borderless: return "Borderless";
            case WindowMode::Fullscreen: return "Fullscreen";
        }

        return "Undefined";
    }
}