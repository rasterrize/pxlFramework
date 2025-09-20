#pragma once

namespace pxl
{
    enum class Ease
    {
        Linear,
        InSine,
        OutSine,
        InOutSine,
        InQuad,
        OutQuad,
        InOutQuad,
        InCubic,
        OutCubic,
        InOutCubic,
        InQuart,
        OutQuart,
        InOutQuart,
        InQuint,
        OutQuint,
        InOutQuint,
        InExpo,
        OutExpo,
        InOutExpo,
        InCirc,
        OutCirc,
        InOutCirc,
        InBack,
        OutBack,
        InOutBack,
        InElastic,
        OutElastic,
        InOutElastic,
        InBounce,
        OutBounce,
        InOutBounce,
    };

    class Easing
    {
    public:
        static std::function<double(double)> GetEasingFunction(Ease ease);
        static double InSine(double t);
        static double OutSine(double t);
        static double InOutSine(double t);
        static double InQuad(double t);
        static double OutQuad(double t);
        static double InOutQuad(double t);
        static double InCubic(double t);
        static double OutCubic(double t);
        static double InOutCubic(double t);
        static double InQuart(double t);
        static double OutQuart(double t);
        static double InOutQuart(double t);
        static double InQuint(double t);
        static double OutQuint(double t);
        static double InOutQuint(double t);
        static double InExpo(double t);
        static double OutExpo(double t);
        static double InOutExpo(double t);
        static double InCirc(double t);
        static double OutCirc(double t);
        static double InOutCirc(double t);
        static double InBack(double t);
        static double OutBack(double t);
        static double InOutBack(double t);
        static double InElastic(double t);
        static double OutElastic(double t);
        static double InOutElastic(double t);
        static double InBounce(double t);
        static double OutBounce(double t);
        static double InOutBounce(double t);
    };
}