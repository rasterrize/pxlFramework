#include "AudioTrack.h"

#include "AudioUtil.h"

namespace pxl
{
    AudioTrack::AudioTrack(HSTREAM stream)
    {
        m_Stream = stream;

        m_LengthInBytes = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
        m_LengthInSeconds = ToSeconds(BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE));

        BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, 0, FinishSyncProc, this);
    }

    AudioTrack::~AudioTrack()
    {
    }

    void AudioTrack::Play()
    {
        BASS_CHECK(BASS_ChannelPlay(m_Stream, false));
    }

    void AudioTrack::Pause()
    {
        BASS_CHECK(BASS_ChannelPause(m_Stream)); // TODO: Check if the track is playing
    }

    void AudioTrack::Restart()
    {
        BASS_CHECK(BASS_ChannelPlay(m_Stream, true));
    }

    void AudioTrack::Stop()
    {
        BASS_CHECK(BASS_ChannelStop(m_Stream)); // TODO: Check if the track is playing
    }

    bool AudioTrack::IsPlaying() const
    {
        if (BASS_ChannelIsActive(m_Stream) != BASS_ACTIVE_PLAYING)
            return false;
        else
            return true;
    }

    void AudioTrack::SetPosition(double seconds)
    {
        BASS_CHECK(BASS_ChannelSetPosition(m_Stream, ToBytes(seconds), BASS_POS_BYTE));
    }

    void AudioTrack::OffsetPosition(double offsetSecs)
    {
        // TODO: clamp current + offset between start and end of stream
        BASS_CHECK(BASS_ChannelSetPosition(m_Stream, GetPositionInBytes() + ToBytes(offsetSecs), BASS_POS_BYTE));
    }

    void AudioTrack::FinishSyncProc(HSYNC handle, DWORD channel, DWORD data, void* user)
    {
        auto& track = *static_cast<AudioTrack*>(user);

        if (track.m_FinishCallback)
            track.m_FinishCallback();
    }

    void AudioTrack::Free()
    {
        BASS_CHECK(BASS_StreamFree(m_Stream));
    }
}