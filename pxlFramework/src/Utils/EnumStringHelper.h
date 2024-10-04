#pragma once

#include "Renderer/RendererAPIType.h"
#include "Core/Window.h"

namespace pxl
{
    class EnumStringHelper
    {
    public:
        static std::string RendererAPITypeToString(RendererAPIType type);
        static std::string ToString(WindowMode mode);
    };
}