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
            m_StartTime = std::chrono::high_resolution_clock::now();
        }

        void Stop()
        {
            m_EndTime = std::chrono::high_resolution_clock::now();
            m_Stopped = true;
            CalculateElapsed();
        }

        void Restart()
        {
            m_StartTime = std::chrono::high_resolution_clock::now();
            m_EndTime = std::chrono::high_resolution_clock::now();
            m_Elapsed = std::chrono::duration<float>::zero();
        }

        float GetElapsedSec() { if (!m_Stopped) { CalculateElapsed(); } return m_Elapsed.count(); }

        float GetElapsedMilliSec() { if (!m_Stopped) { CalculateElapsed(); } return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(m_Elapsed).count()); }

        float GetElapsedMicroSec() { if (!m_Stopped) { CalculateElapsed(); } return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_Elapsed).count()); }

    private:
        void CalculateElapsed()
        {
            if (m_Stopped)
                m_Elapsed = m_EndTime - m_StartTime;
            else
                m_Elapsed = std::chrono::high_resolution_clock::now() - m_StartTime;
        }
    
    private:
        bool m_Stopped = false;

        std::chrono::high_resolution_clock::time_point m_StartTime, m_EndTime;
        std::chrono::duration<float> m_Elapsed;
    }; 
}