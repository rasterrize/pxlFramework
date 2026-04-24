#pragma once

namespace pxl
{
    template<typename T>
    struct Rect
    {
        T Left = {};
        T Right = {};
        T Bottom = {};
        T Top = {};
    };

    using RectF = Rect<float>;
}