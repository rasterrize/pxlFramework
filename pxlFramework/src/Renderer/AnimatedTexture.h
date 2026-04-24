#pragma once

#include "Core/Stopwatch.h"
#include "Texture.h"

namespace pxl
{
    struct AnimatedTexture
    {
        std::vector<SubTexture> Frames;
        uint32_t Framerate = 24;
        float PlaybackSpeed = 1.0f;
        uint32_t FrameIndex = 0;
        Stopwatch m_Stopwatch = {};

        void Start() { m_Stopwatch.Start(); }
        void Stop() { m_Stopwatch.Stop(); }
        void Reset() { m_Stopwatch.Reset(); }

        const SubTexture& GetCurrentFrame()
        {
            float msPerFrame = 1000.0f / (static_cast<float>(Framerate) * PlaybackSpeed);
            uint32_t framesElapsed = static_cast<uint32_t>(m_Stopwatch.GetElapsedMilliSec() / msPerFrame);
            if (framesElapsed > 0)
            {
                FrameIndex = std::max(0u, (FrameIndex + framesElapsed) % static_cast<uint32_t>(Frames.size()));
                m_Stopwatch.Reset();
            }

            return Frames.at(FrameIndex);
        }
    };
}