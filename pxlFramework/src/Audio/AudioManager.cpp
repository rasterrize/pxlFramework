#include "AudioManager.h"
#include "AudioUtil.h"

namespace pxl
{
    bool AudioManager::s_Enabled = false;

    int AudioManager::s_CurrentDeviceIndex = -1; // -1 = default device
    int AudioManager::s_Frequency = 44100;       // considered the standard for human hearing, can be increased for better fx
    float AudioManager::s_Volume = 10;

    std::shared_ptr<Window> AudioManager::s_WindowHandle = nullptr;

    std::unordered_map<std::string, std::shared_ptr<AudioTrack>> AudioManager::s_Tracks;

    void AudioManager::Init(const std::shared_ptr<Window> windowHandle = nullptr)
    {
        s_WindowHandle = windowHandle;

        if (BASS_Init(s_CurrentDeviceIndex, s_Frequency, 0, NULL, NULL))
        {
            PXL_LOG_INFO(LogArea::Audio, "BASS initialized");
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Audio, "BASS failed to initialize");
            // TODO: bass error callbacks
            return;
        }

        // Configure BASS
        BASS_CHECK(BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(s_Volume * 100))); // Stream Volume in BASS is from 0 - 10000

        s_Enabled = true;
    }

    void AudioManager::Shutdown()
    {
        if (!s_Enabled)
            return;

        BASS_CHECK(BASS_Free());
        s_Enabled = false;
    }

    void AudioManager::Add(const std::string& trackName, const std::shared_ptr<AudioTrack> track)
    {
        s_Tracks.emplace(trackName, track);
    }

    void AudioManager::Play(const std::string& trackName)
    {
        s_Tracks.at(trackName)->Play(); // TODO: this and the 2 functions below should check if the track exists
    }

    void AudioManager::Pause(const std::string& trackName)
    {
        s_Tracks.at(trackName)->Pause();
    }

    void AudioManager::Stop(const std::string& trackName)
    {
        s_Tracks.at(trackName)->Stop();
    }

    std::vector<std::string> AudioManager::GetLibrary()
    {
        std::vector<std::string> audioNames;
        audioNames.reserve(s_Tracks.size());

        for (auto& [name, track] : s_Tracks)
        {
            audioNames.push_back(name);
        }

        return audioNames;
    }
}