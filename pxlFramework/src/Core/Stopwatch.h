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
            m_StartPoint = std::chrono::steady_clock::now();
            m_Running = true;
        }

        void Stop()
        {
            CalculateElapsed();
            m_Running = false;
        }

        void Reset()
        {
            m_StartPoint = std::chrono::steady_clock::now();
            m_Elapsed = std::chrono::duration<double>::zero();
        }

        double GetElapsedSec()
        {
            CalculateElapsed();
            return m_Elapsed.count();
        }

        double GetElapsedMilliSec()
        {
            CalculateElapsed();
            return std::chrono::duration<double, std::milli>(m_Elapsed).count();
        }

        double GetElapsedMicroSec()
        {
            CalculateElapsed();
            return std::chrono::duration<double, std::micro>(m_Elapsed).count();
        }

        double GetElapsedNanoSec()
        {
            CalculateElapsed();
            return std::chrono::duration<double, std::nano>(m_Elapsed).count();
        }

        bool IsRunning() { return m_Running; }

    private:
        void CalculateElapsed()
        {
            if (!m_Running)
                return;

            m_Elapsed = std::chrono::steady_clock::now() - m_StartPoint;
        }

    private:
        bool m_Running = false;

        std::chrono::steady_clock::time_point m_StartPoint = {};
        std::chrono::duration<double> m_Elapsed = std::chrono::duration<double>::zero();
    };
}