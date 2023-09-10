#include "AudioManager.h"
#include "../Core/Window/WindowGLFW.h"

namespace pxl
{
    bool AudioManager::s_Enabled = false;

    int AudioManager::s_CurrentDeviceIndex = 1;
    int AudioManager::s_Frequency = 44100;
    float AudioManager::s_Volume = 10; // should always be 0-1

    std::shared_ptr<Window> AudioManager::s_WindowHandle = nullptr;

    std::unordered_map<std::string, std::shared_ptr<AudioTrack>> AudioManager::s_Tracks;

    void AudioManager::Init(const std::shared_ptr<Window> windowHandle)
    {
        s_WindowHandle = windowHandle;

        if (BASS_Init(s_CurrentDeviceIndex, s_Frequency, 0, windowHandle->GetHWND(), NULL))
        {
            Logger::LogInfo("BASS Initalized");
        }
        else
        {
            Logger::LogError("BASS failed to initialize");
            return;
        }

        // Configure BASS
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, s_Volume * 100);

        s_Enabled = true;
    }

    void AudioManager::Shutdown()
    {
        if (!s_Enabled)
            return;
        
        BASS_Free();
        s_Enabled = false;
    }

    void AudioManager::Add(const std::string& trackName, const std::shared_ptr<AudioTrack> track)
    {
        s_Tracks.emplace(trackName, track);
    }

    void AudioManager::Play(const std::string& trackName)
    {
        s_Tracks.at(trackName)->Play();
    }

    void AudioManager::Pause(const std::string &trackName)
    {
        s_Tracks.at(trackName)->Pause();
    }

    void AudioManager::Stop(const std::string &trackName)
    {
        s_Tracks.at(trackName)->Stop();
    }

    std::vector<std::string> AudioManager::GetLibrary()
    {
        std::vector<std::string> audioNames;
        
        for (auto track : s_Tracks)
        {
            audioNames.push_back(track.first);
        }

        return audioNames;
    }
}