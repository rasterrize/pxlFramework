#include "AudioTrack.h"

#include "AudioUtil.h"

namespace pxl
{
    AudioTrack::AudioTrack(HSTREAM stream)
    {
        m_Stream = stream;
    }

    AudioTrack::~AudioTrack()
    {
        Free();
    }

    void AudioTrack::Play()
    {
        BASS_CHECK(BASS_ChannelPlay(m_Stream, false));
        m_Playing = true;
    }

    void AudioTrack::Pause()
    {
        BASS_CHECK(BASS_ChannelPause(m_Stream)); // TODO: Check if the track is playing
        m_Playing = false;
    }

    void AudioTrack::Stop()
    {
        BASS_CHECK(BASS_ChannelStop(m_Stream)); // TODO: Check if the track is playing
        m_Playing = false;
    }

    void AudioTrack::Free()
    {
        BASS_CHECK(BASS_StreamFree(m_Stream));
    }
}