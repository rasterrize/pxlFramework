#include "Easing.h"

#include <cmath>
#include <numbers>

namespace pxl
{
    // ---------------------------------------------------------------------------
    // These easing function implementations have been sourced from:
    // https://github.com/nicolausYes/easing-functions/blob/master/src/easing.cpp
    //
    // For visualizations or more information on these, visit: https://easings.net/
    // ---------------------------------------------------------------------------

    static constexpr double pi = std::numbers::pi;

    std::function<double(double)> Easing::GetEasingFunction(Ease ease)
    {
        switch (ease)
        {
            case Ease::InSine: return InSine;
            case Ease::OutSine: return OutSine;
            case Ease::InOutSine: return InOutSine;
            case Ease::InQuad: return InQuad;
            case Ease::OutQuad: return OutQuad;
            case Ease::InOutQuad: return InOutQuad;
            case Ease::InCubic: return InCubic;
            case Ease::OutCubic: return OutCubic;
            case Ease::InOutCubic: return InOutCubic;
            case Ease::InQuart: return InQuart;
            case Ease::OutQuart: return OutQuart;
            case Ease::InOutQuart: return InOutQuart;
            case Ease::InQuint: return InQuint;
            case Ease::OutQuint: return OutQuint;
            case Ease::InOutQuint: return InOutQuint;
            case Ease::InExpo: return InExpo;
            case Ease::OutExpo: return OutExpo;
            case Ease::InOutExpo: return InOutExpo;
            case Ease::InCirc: return InCirc;
            case Ease::OutCirc: return OutCirc;
            case Ease::InOutCirc: return InOutCirc;
            case Ease::InBack: return InBack;
            case Ease::OutBack: return OutBack;
            case Ease::InOutBack: return InOutBack;
            case Ease::InElastic: return InElastic;
            case Ease::OutElastic: return OutElastic;
            case Ease::InOutElastic: return InOutElastic;
            case Ease::InBounce: return InBounce;
            case Ease::OutBounce: return OutBounce;
            case Ease::InOutBounce: return InOutBounce;
        }

        return [](double) { return 0.0f; };
    }

    double Easing::InSine(double t)
    {
        return sin(1.5707963 * t);
    }

    double Easing::OutSine(double t)
    {
        return 1 + sin(1.5707963 * (--t));
    }

    double Easing::InOutSine(double t)
    {
        return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
    }

    double Easing::InQuad(double t)
    {
        return t * t;
    }

    double Easing::OutQuad(double t)
    {
        return t * (2 - t);
    }

    double Easing::InOutQuad(double t)
    {
        return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
    }

    double Easing::InCubic(double t)
    {
        return t * t * t;
    }

    double Easing::OutCubic(double t)
    {
        return 1 + (--t) * t * t;
    }

    double Easing::InOutCubic(double t)
    {
        return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
    }

    double Easing::InQuart(double t)
    {
        t *= t;
        return t * t;
    }

    double Easing::OutQuart(double t)
    {
        t = (--t) * t;
        return 1 - t * t;
    }

    double Easing::InOutQuart(double t)
    {
        if (t < 0.5)
        {
            t *= t;
            return 8 * t * t;
        }
        else
        {
            t = (--t) * t;
            return 1 - 8 * t * t;
        }
    }

    double Easing::InQuint(double t)
    {
        double t2 = t * t;
        return t * t2 * t2;
    }

    double Easing::OutQuint(double t)
    {
        double t2 = (--t) * t;
        return 1 + t * t2 * t2;
    }

    double Easing::InOutQuint(double t)
    {
        double t2;
        if (t < 0.5)
        {
            t2 = t * t;
            return 16 * t * t2 * t2;
        }
        else
        {
            t2 = (--t) * t;
            return 1 + 16 * t * t2 * t2;
        }
    }

    double Easing::InExpo(double t)
    {
        return (pow(2, 8 * t) - 1) / 255;
    }

    double Easing::OutExpo(double t)
    {
        return 1 - pow(2, -8 * t);
    }

    double Easing::InOutExpo(double t)
    {
        if (t < 0.5)
        {
            return (pow(2, 16 * t) - 1) / 510;
        }
        else
        {
            return 1 - 0.5 * pow(2, -16 * (t - 0.5));
        }
    }

    double Easing::InCirc(double t)
    {
        return 1 - sqrt(1 - t);
    }

    double Easing::OutCirc(double t)
    {
        return sqrt(t);
    }

    double Easing::InOutCirc(double t)
    {
        if (t < 0.5)
        {
            return (1 - sqrt(1 - 2 * t)) * 0.5;
        }
        else
        {
            return (1 + sqrt(2 * t - 1)) * 0.5;
        }
    }

    double Easing::InBack(double t)
    {
        return t * t * (2.70158 * t - 1.70158);
    }

    double Easing::OutBack(double t)
    {
        return 1 + (--t) * t * (2.70158 * t + 1.70158);
    }

    double Easing::InOutBack(double t)
    {
        if (t < 0.5)
        {
            return t * t * (7 * t - 2.5) * 2;
        }
        else
        {
            return 1 + (--t) * t * 2 * (7 * t + 2.5);
        }
    }

    double Easing::InElastic(double t)
    {
        double t2 = t * t;
        return t2 * t2 * sin(t * pi * 4.5);
    }

    double Easing::OutElastic(double t)
    {
        double t2 = (t - 1) * (t - 1);
        return 1 - t2 * t2 * cos(t * pi * 4.5);
    }

    double Easing::InOutElastic(double t)
    {
        double t2;
        if (t < 0.45)
        {
            t2 = t * t;
            return 8 * t2 * t2 * sin(t * pi * 9);
        }
        else if (t < 0.55)
        {
            return 0.5 + 0.75 * sin(t * pi * 4);
        }
        else
        {
            t2 = (t - 1) * (t - 1);
            return 1 - 8 * t2 * t2 * sin(t * pi * 9);
        }
    }

    double Easing::InBounce(double t)
    {
        return pow(2, 6 * (t - 1)) * abs(sin(t * pi * 3.5));
    }

    double Easing::OutBounce(double t)
    {
        return 1 - pow(2, -6 * t) * abs(cos(t * pi * 3.5));
    }

    double Easing::InOutBounce(double t)
    {
        if (t < 0.5)
        {
            return 8 * pow(2, 8 * (t - 1)) * abs(sin(t * pi * 7));
        }
        else
        {
            return 1 - 8 * pow(2, -8 * t) * abs(sin(t * pi * 7));
        }
    }
}