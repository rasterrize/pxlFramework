#include "AudioTrack.h"

#include "AudioUtil.h"

namespace pxl
{
    AudioTrack::AudioTrack(HSTREAM stream, const AudioMetadata& metadata)
        : m_Metadata(metadata)
    {
        m_Stream = stream;

        m_LengthInBytes = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
        m_LengthInSeconds = ToSeconds(BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE));

        BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, 0, FinishSyncProc, this);

        // Get metadata
        // TODO: this only works for flac, need alternative functions for different file types

        auto comments = BASS_ChannelGetTags(m_Stream, BASS_TAG_OGG);
        if (comments)
        {
            while (*comments)
            {
                std::string string(comments);
                auto equalsPos = string.find_first_of("=");
                auto type = string.substr(0, equalsPos);
                auto value = string.substr(equalsPos + 1, string.length());

                if (type == "TITLE")
                    m_Metadata.value().Title = value;

                if (type == "ARTIST")
                    m_Metadata.value().Artist = value;

                if (type == "ALBUM")
                    m_Metadata.value().Album = value;

                if (type == "GENRE")
                    m_Metadata.value().Genres.push_back(value);

                comments += strlen(comments) + 1;
            }
        }
    }

    AudioTrack::~AudioTrack()
    {
        if (m_Stream)
            Free();
    }

    void AudioTrack::Play(bool restart)
    {
        BASS_CHECK(BASS_ChannelPlay(m_Stream, restart));
    }

    void AudioTrack::Pause()
    {
        BASS_CHECK(BASS_ChannelPause(m_Stream));
    }

    bool AudioTrack::IsPlaying() const
    {
        return (BASS_ChannelIsActive(m_Stream) == BASS_ACTIVE_PLAYING);
    }

    void AudioTrack::Preload(uint32_t ms)
    {
        BASS_CHECK(BASS_ChannelUpdate(m_Stream, ms));
    }

    void AudioTrack::SetPosition(double seconds)
    {
        BASS_CHECK(BASS_ChannelSetPosition(m_Stream, ToBytes(ClampSeconds(seconds)), BASS_POS_BYTE));
    }

    void AudioTrack::OffsetPosition(double offsetSecs)
    {
        BASS_CHECK(BASS_ChannelSetPosition(m_Stream, ToBytes(ClampSeconds(GetPosition() + offsetSecs)), BASS_POS_BYTE));
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

    float AudioTrack::GetAttribute(DWORD attribute) const
    {
        float value;
        BASS_CHECK(BASS_ChannelGetAttribute(m_Stream, attribute, &value));
        return value;
    }

    void AudioTrack::SetAttribute(DWORD attrib, float value)
    {
        BASS_CHECK(BASS_ChannelSetAttribute(m_Stream, attrib, value));
    }

    double AudioTrack::ClampSeconds(double secs)
    {
        // NOTE: Clamps below actual length to avoid errors
        return std::clamp(secs, 0.0, m_LengthInSeconds - 0.00001);
    }
}