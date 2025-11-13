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

    template<typename T>
    class Animation
    {
    public:
        Animation(Ease easing, float duration)
            : m_Easing(easing), m_Duration(duration), m_EasingFunction(Easing::GetEasingFunction(easing))
        {
        }

        Animation(Ease easing, float duration, T startValue, T endValue)
            : m_Easing(easing), m_Duration(duration), m_EasingFunction(Easing::GetEasingFunction(easing)), m_StartValue(startValue), m_EndValue(endValue)
        {
        }

        // Starts the animation at its current point
        void Start() { m_Stopwatch.Start(); }

        // Stops the animation at its current point
        void Stop() { m_Stopwatch.Stop(); }

        // Stops the animation and resets it to its starting point
        void Reset()
        {
            Stop();
            m_Stopwatch.Reset();
        }

        // Starts the animation again from its starting point
        void Restart()
        {
            Reset();
            Start();
        }

        // Progress of the eased animation from 0.0 -> 1.0
        float GetProgress()
        {
            float elapsed = std::min<float>(m_Stopwatch.GetElapsedMilliSec(), m_Duration);
            return m_Easing == Ease::Linear ? GetLinearProgress() : std::min<float>(static_cast<float>(m_EasingFunction(elapsed / m_Duration)), 1.0f);
        }

        // Linear progress of the animation without any easing applied from 0.0 -> 1.0
        float GetLinearProgress() { return std::min<float>(m_Stopwatch.GetElapsedMilliSec() / m_Duration, 1.0f); }

        // Progress of the animation given between defined start and end values.
        float GetProgressValue() { return m_StartValue + ((m_EndValue - m_StartValue) * GetProgress()); }

        T GetStartValue() const { return m_StartValue; }
        void SetStartValue(T value) { m_StartValue = value; }

        T GetEndValue() const { return m_EndValue; }
        void SetEndValue(T value) { m_EndValue = value; }

        // Sets the easing function from predefined easing type
        void SetEasingFunc(Ease ease) { m_EasingFunction = Easing::GetEasingFunction(ease); }

        // Set the easing function using a custom 0 to 1 algorithm
        void SetEasingFunc(const std::function<double(double)>& func) { m_EasingFunction = func; }

        Stopwatch GetStopwatch() const { return m_Stopwatch; }

        bool IsComplete() { return m_Stopwatch.GetElapsedMilliSec() >= m_Duration; }

    private:
        Ease m_Easing = Ease::Linear;

        Stopwatch m_Stopwatch = Stopwatch(false);

        float m_Duration = 1000.0f;

        T m_StartValue;
        T m_EndValue;

        std::function<double(double)> m_EasingFunction;
#if 0
        uint32_t m_CurrentSubAnimationIndex = 0;
        SubAnimation m_SubAnimation;
        std::vector<SubAnimation> m_SubAnimations;
#endif
    };
}