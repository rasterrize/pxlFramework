// represents an audio track (mp3, flac, etc)
#pragma once

#include <bass.h>

namespace pxl
{
    class AudioTrack
    {
    public:
        AudioTrack(HSTREAM stream);
        ~AudioTrack();

        void Play();
        void Pause();
        void Stop();

        const HSTREAM GetStream() const { return m_Stream; }
    private:
        void Free();
    private:
        HSTREAM m_Stream;

        bool m_IsPlaying = false;
    };
}