#pragma once

#include "AudioTrack.h"
#include "Core/Window.h"

namespace pxl
{
    struct AudioConfig
    {
        int CurrentDeviceIndex = -1; // -1 = default device
        int Frequency = 44100;       // Considered the standard for human hearing, can be increased for better fx
        float MasterVolume = 10;     // Percentage
    };

    class AudioManager
    {
    public:
        static bool Init(const AudioConfig& config = {}, std::shared_ptr<Window> window = nullptr);
        static void Shutdown();

        static void Add(const std::string& trackName, const std::shared_ptr<AudioTrack> track);
        static std::shared_ptr<AudioTrack> Get(const std::string& trackName) { return s_Tracks.at(trackName); }

        static std::unordered_map<std::string, std::shared_ptr<AudioTrack>> GetTrackLibrary() { return s_Tracks; }

        static void SetMasterVolume(float percentage);
        static void AdjustMasterVolume(float offset);

    private:
        static inline bool s_Enabled = false;

        static inline AudioConfig s_Config = {};

        // Only needed for DirectSound output
        static inline std::shared_ptr<Window> s_WindowHandle = nullptr;

        static inline std::unordered_map<std::string, std::shared_ptr<AudioTrack>> s_Tracks;
    };
}