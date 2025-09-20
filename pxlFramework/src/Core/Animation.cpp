#include "Animation.h"

namespace pxl
{
    Animation::Animation(Ease easing, float duration, bool loop)
        : m_Easing(easing), m_Duration(duration), m_EasingFunction(Easing::GetEasingFunction(easing))
    {

    }
}