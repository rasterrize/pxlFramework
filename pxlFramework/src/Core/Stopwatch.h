#pragma once

namespace pxl
{
    class Stopwatch
    {
    public:
        Stopwatch(bool start = true)
        {
            if (start)
                Start();
        }

        ~Stopwatch()
        {
            Stop();
        }

        void Start()
        {
            m_LastStartPoint = std::chrono::high_resolution_clock::now();
            m_Running = true;
        }

        void Stop()
        {
            CalculateElapsed();
            m_Running = false;
        }

        void Reset()
        {
            m_LastStartPoint = std::chrono::high_resolution_clock::now();
            m_Elapsed = std::chrono::duration<float>::zero();
        }

        float GetElapsedSec()
        {
            CalculateElapsed();
            return m_Elapsed.count();
        }

        float GetElapsedMilliSec()
        {
            CalculateElapsed();
            return GetElapsed<std::chrono::milliseconds, float>();
        }

        float GetElapsedMicroSec()
        {
            CalculateElapsed();
            return GetElapsed<std::chrono::microseconds, float>();
        }

        bool IsRunning() { return m_Running; }

    private:
        void CalculateElapsed()
        {
            if (!m_Running)
                return;

            m_Elapsed += std::chrono::high_resolution_clock::now() - m_LastStartPoint;
            m_LastStartPoint = std::chrono::high_resolution_clock::now();
        }

        template<typename timeT, typename outputT>
        outputT GetElapsed()
        {
            return static_cast<outputT>(std::chrono::duration_cast<timeT>(m_Elapsed).count());
        }

    private:
        bool m_Running = false;

        std::chrono::high_resolution_clock::time_point m_LastStartPoint;
        std::chrono::duration<float> m_Elapsed;
    };
}