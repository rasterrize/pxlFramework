#include "AudioTrack.h"

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
        BASS_ChannelPlay(m_Stream, FALSE);
    }

    void AudioTrack::Pause()
    {
        BASS_ChannelPause(m_Stream); // TODO: Check if the track is playing
    }

    void AudioTrack::Stop()
    {
        BASS_ChannelStop(m_Stream); // TODO: Check if the track is playing
    }

    void AudioTrack::Free()
    {
        BASS_StreamFree(m_Stream);
    }
}