#pragma once

#include <glm/glm.hpp>

namespace pxl
{
    enum class ColourName
    {
        White,
        Black,
        Red,
        Green,
        Blue,
        Pink,
        Yellow,
        // TODO: more
    };

    class Colour
    {
    public:
        static glm::vec4 GetColourAsVec4(ColourName colour)
        {
            switch (colour)
            {
                case ColourName::Black:     return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                case ColourName::White:     return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                case ColourName::Red:       return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                case ColourName::Green:     return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                case ColourName::Blue:      return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                case ColourName::Yellow:    return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            }
        
            PXL_LOG_WARN(LogArea::Core, "Invalid pxlColour");

            return glm::vec4(1.0f);
        }
    };
}