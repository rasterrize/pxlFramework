#pragma once

#include "Stopwatch.h"

#include "Utils/Easing.h"

namespace pxl
{
#if 0
    struct SubAnimation
    {
        Ease Easing = Ease::Linear;

        // The duration of the animation in milliseconds
        float Duration = 1000;

        Stopwatch Elapsed = Stopwatch(false);
    };
#endif

    class Animation
    {
    public:
        Animation(Ease easing, float duration = 1000.0f, bool loop = false);

        void Start() { m_Stopwatch.Start(); }
        void Stop() { m_Stopwatch.Stop(); }
        void Reset() { m_Stopwatch.Reset(); }

        // Progress of the eased animation from 0 -> 1
        float GetProgress() { return std::min<float>(static_cast<float>(m_EasingFunction(m_Stopwatch.GetElapsedMilliSec() / m_Duration)), 1.0f); }

        // Linear progress of the animation without any easing applied from 0 -> 1
        float GetLinearProgress() { return std::min<float>(m_Stopwatch.GetElapsedMilliSec() / m_Duration, 1.0f); }

        Stopwatch GetStopwatch() { return m_Stopwatch; }

        bool IsComplete() { return m_Stopwatch.GetElapsedMilliSec() >= m_Duration; }
    private:
        // TODO: Linear
        Ease m_Easing = Ease::InSine; 

        Stopwatch m_Stopwatch;

        float m_Duration = 0.0f;

        std::function<double(double)> m_EasingFunction;

        // uint32_t m_CurrentSubAnimationIndex = 0;
        // SubAnimation m_SubAnimation;
        // std::vector<SubAnimation> m_SubAnimations;
    };
}