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
        void Restart();
        void Stop();

        // Get the current track's position in seconds
        double GetPosition() const { return ToSeconds(GetPositionInBytes()); }

        void SetPosition(double seconds);
        void OffsetPosition(double seconds);

        bool IsPlaying() const;

        void SetFinishCallback(const std::function<void()>& func) { m_FinishCallback = func; }

        const HSTREAM GetStream() const { return m_Stream; }

        static void FinishSyncProc(HSYNC handle, DWORD channel, DWORD data, void* user);

    private:
        void Free();

        QWORD GetPositionInBytes() const { return BASS_ChannelGetPosition(m_Stream, BASS_POS_BYTE); }

        double ToSeconds(QWORD bytes) const { return BASS_ChannelBytes2Seconds(m_Stream, bytes); }
        QWORD ToBytes(double seconds) const { return BASS_ChannelSeconds2Bytes(m_Stream, seconds); }

    private:
        HSTREAM m_Stream;

        QWORD m_LengthInBytes;
        double m_LengthInSeconds;

        std::function<void()> m_PauseCallback;
        std::function<void()> m_FinishCallback;
    };
}