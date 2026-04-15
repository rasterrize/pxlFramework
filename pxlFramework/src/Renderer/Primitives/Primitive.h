#pragma once

namespace pxl
{
    enum class Origin2D
    {
        Centre,
        CentreLeft,
        CentreRight,
        TopLeft,
        TopCentre,
        TopRight,
        BottomLeft,
        BottomCentre,
        BottomRight,
    };

    enum class Anchor2D
    {
        None,
        Centre,
        CentreLeft,
        CentreRight,
        TopLeft,
        TopCentre,
        TopRight,
        BottomLeft,
        BottomCentre,
        BottomRight,
    };

    enum class Scaling2D
    {
        Absolute = 0,
        RelativeX,
        RelativeY,
        RelativeBoth,
        ScaleUpToFit,
        ScaleDownToFit,
    };

    namespace Utils
    {
        inline float RotationWrap(float rotation)
        {
            rotation = std::fmod(rotation, 360.0f);

            if (rotation < 0.0f)
                rotation = std::fmod(rotation, -360.0f);

            return rotation;
        }
    }
}