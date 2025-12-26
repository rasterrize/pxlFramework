// represents an audio track (mp3, flac, etc)
#pragma once

#include <bass.h>

namespace pxl
{
    struct AudioMetadata
    {
        std::string Title;
        std::string Artist;
        std::vector<std::string> Genres;
        std::string Album;
    };

    class AudioTrack
    {
    public:
        AudioTrack(HSTREAM stream, const AudioMetadata& metadata = {});
        ~AudioTrack();

        void Play(bool restart = false);
        void Pause();

        // Get the current track's position in seconds
        double GetPosition() const { return ToSeconds(GetPositionInBytes()); }

        void SetPosition(double seconds);
        void OffsetPosition(double seconds);

        float GetVolume() const { return GetAttribute(BASS_ATTRIB_VOL); }

        // 0.0 -> 1.0 = normal volume, 1.0+ = amplification
        void SetVolume(float value) { SetAttribute(BASS_ATTRIB_VOL, value); }

        bool IsPlaying() const;

        void SetFinishCallback(const std::function<void()>& func) { m_FinishCallback = func; }

        const HSTREAM GetStream() const { return m_Stream; }

        static void FinishSyncProc(HSYNC handle, DWORD channel, DWORD data, void* user);

    private:
        void Free();

        QWORD GetPositionInBytes() const { return BASS_ChannelGetPosition(m_Stream, BASS_POS_BYTE); }

        double ToSeconds(QWORD bytes) const { return BASS_ChannelBytes2Seconds(m_Stream, bytes); }
        QWORD ToBytes(double seconds) const { return BASS_ChannelSeconds2Bytes(m_Stream, seconds); }

        float GetAttribute(DWORD attrib) const;
        void SetAttribute(DWORD attrib, float value);

        double ClampSeconds(double secs);

    private:
        HSTREAM m_Stream;

        QWORD m_LengthInBytes;
        double m_LengthInSeconds;

        std::optional<AudioMetadata> m_Metadata;

        std::function<void()> m_PauseCallback;
        std::function<void()> m_FinishCallback;
    };
}