#pragma once

#include "Renderer/RendererAPIType.h"

namespace pxl
{
    class EnumStringHelper
    {
    public:
        static std::string RendererAPITypeToString(RendererAPIType type);
    };
}