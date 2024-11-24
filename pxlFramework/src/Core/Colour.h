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
    };

    class Colour
    {
    public:
        static glm::vec4 AsVec4(ColourName colour)
        {
            switch (colour)
            {
                case ColourName::Black:  return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                case ColourName::White:  return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                case ColourName::Red:    return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                case ColourName::Green:  return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                case ColourName::Blue:   return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                case ColourName::Pink:   return glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
                case ColourName::Yellow: return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            }

            PXL_LOG_WARN(LogArea::Core, "Invalid colour name, returning white");

            return glm::vec4(1.0f);
        }
    };
}