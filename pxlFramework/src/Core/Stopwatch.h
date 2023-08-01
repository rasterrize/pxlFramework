#include <chrono>

namespace pxl
{
    class Stopwatch
    {
    public:
        Stopwatch()
            : m_StartTime(std::chrono::high_resolution_clock::now()) {}

        ~Stopwatch() { Stop(); }

        void Stop()
        {
            m_EndTime = std::chrono::high_resolution_clock::now();
            m_Elapsed = m_EndTime - m_StartTime;
            m_Stopped = true;
        }

        float GetElapsedSec() { if (!m_Stopped) { CalculateElapsed(); } return m_Elapsed.count(); }

        float GetElapsedMS() { if (!m_Stopped) { CalculateElapsed(); } return std::chrono::duration_cast<std::chrono::milliseconds>(m_Elapsed).count(); }

    private:
        void CalculateElapsed()
        {
            m_Elapsed = m_StartTime - std::chrono::high_resolution_clock::now();
        }
    
    private:
        bool m_Stopped = false;

        std::chrono::steady_clock::time_point m_StartTime, m_EndTime;
        std::chrono::duration<float> m_Elapsed;
    }; 
}