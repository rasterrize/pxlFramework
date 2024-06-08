#pragma once

#include <random>

namespace pxl
{
    class Random
    {
    public:
        static float Float(float min = FLT_MIN, float max = FLT_MAX);
        static double Double(double min = DBL_MIN, double max = DBL_MAX);
        static uint32_t UInt(uint32_t min = 0, uint32_t max = UINT32_MAX);
        static int32_t Int(int32_t min = INT32_MIN, int32_t max = INT32_MAX);

    private:
        static uint32_t GenerateSeed();

    private:
        static inline std::mt19937 s_RandomEngine;
        static inline uint32_t s_Seed;
    };
}