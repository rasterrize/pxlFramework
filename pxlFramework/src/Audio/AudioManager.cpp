#include "AudioManager.h"

#include "AudioUtil.h"

namespace pxl
{
    bool AudioManager::Init(const AudioConfig& config, std::shared_ptr<Window> window)
    {
        s_Config = config;

        if (BASS_Init(s_Config.CurrentDeviceIndex, s_Config.Frequency, 0, NULL, NULL))
        {
            PXL_LOG_INFO(LogArea::Audio, "BASS initialized");
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Audio, "BASS failed to initialize");
            return false;
        }

        SetMasterVolume(s_Config.MasterVolume);

        s_Config.CurrentDeviceIndex = BASS_GetDevice();

        BASS_PluginLoad("bassflac", 0);

        BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 5);

        s_Enabled = true;

        return true;
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

    void AudioManager::SetMasterVolume(float percentage)
    {   
        s_Config.MasterVolume = std::clamp(percentage, 0.0f, 100.0f);
        BASS_CHECK(BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(s_Config.MasterVolume * 100.0f))); // Stream Volume in BASS is from 0 - 10000
    }

    void AudioManager::AdjustMasterVolume(float offset)
    {
        s_Config.MasterVolume = std::clamp(s_Config.MasterVolume + offset, 0.0f, 100.0f);
        BASS_CHECK(BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(s_Config.MasterVolume * 100.0f)));
    }
}