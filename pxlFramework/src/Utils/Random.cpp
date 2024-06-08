
#include "Random.h"

#include "Core/Platform.h"

namespace pxl
{
    float Random::Float(float min, float max)
    {
        s_Seed = GenerateSeed();
        s_RandomEngine.seed(s_Seed);
        auto distribution = std::uniform_real_distribution<float>(min, max);
        return distribution(s_RandomEngine);
    }

    double Random::Double(double min, double max)
    {
        s_Seed = GenerateSeed();
        s_RandomEngine.seed(s_Seed);
        auto distribution = std::uniform_real_distribution<double>(min, max);
        return distribution(s_RandomEngine);
    }

    uint32_t Random::UInt(uint32_t min, uint32_t max)
    {
        s_Seed = GenerateSeed();
        s_RandomEngine.seed(s_Seed);
        auto distribution = std::uniform_int_distribution<uint32_t>(min, max);
        return distribution(s_RandomEngine);
    }

    int32_t Random::Int(int32_t min, int32_t max)
    {
        s_Seed = GenerateSeed();
        s_RandomEngine.seed(s_Seed);
        auto distribution = std::uniform_int_distribution<int32_t>(min, max);
        return distribution(s_RandomEngine);
    }

    uint32_t Random::GenerateSeed()
    {
        // NOTE: The seed generation I chucked together is probably either incorrect
        // or unnecessarily complex, but I'm not a mathematician and it's good enough for now.
        return static_cast<uint32_t>(Platform::GetTime() * 1000.0f + s_Seed + std::random_device()());
    }
}