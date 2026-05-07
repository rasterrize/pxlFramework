#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pxl
{
    /// @brief A 2D uint vector represented as width and height.
    struct Size2D
    {
        uint32_t Width = 0;
        uint32_t Height = 0;

        Size2D() = default;

        Size2D(uint32_t width, uint32_t height)
            : Width(width), Height(height)
        {
        }

        Size2D(uint32_t scale)
            : Width(scale), Height(scale)
        {
        }

        bool IsZero() const { return Width == 0 && Height == 0; }
        bool HasZero() const { return Width == 0 || Height == 0; }

        glm::vec2 ToVec2() const { return { Width, Height }; }

        std::string ToString() const { return std::format("{}, {}", Width, Height); }
    };

    /// @brief A 3D uint vector represented as width, height, and depth.
    struct Size3D
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 0;

        Size3D() = default;

        Size3D(uint32_t width, uint32_t height, uint32_t depth)
            : Width(width), Height(height), Depth(depth)
        {
        }

        Size3D(uint32_t scale)
            : Width(scale), Height(scale), Depth(scale)
        {
        }

        bool IsZero() const { return Width == 0 && Height == 0 && Depth == 0; }
        bool HasZero() const { return Width == 0 || Height == 0 || Depth == 0; }

        glm::vec3 ToVec3() const { return { Width, Height, Depth }; }

        std::string ToString() const { return std::format("{}, {}, {}", Width, Height, Depth); }
    };
}