#pragma once

namespace pxl
{
    struct Size2D
    {        
        uint32_t Width = 0;
        uint32_t Height = 0;

        bool IsZero() const { return Width == 0 && Height == 0; }
    };

    struct Size3D
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 0;

        bool IsZero() const { return Width == 0 && Height == 0 && Depth == 0; }
    };
}