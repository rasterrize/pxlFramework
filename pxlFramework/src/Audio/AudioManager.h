#pragma once

#include "AudioTrack.h"
#include "Core/Window.h"

namespace pxl
{
    class AudioManager
    {
    public:
        static void Init(const std::shared_ptr<Window> windowHandle);
        static void Shutdown();

        static void Add(const std::string& trackName, const std::shared_ptr<AudioTrack> track);

        static void Play(const std::string& trackName);
        static void Pause(const std::string& trackName);
        static void Stop(const std::string& trackName);

        static std::vector<std::string> GetLibrary(); // probably should have a different name
    private:
        static bool s_Enabled;

        static int s_CurrentDeviceIndex;
        static int s_Frequency;
        static float s_Volume;

        static std::shared_ptr<Window> s_WindowHandle;

        static std::unordered_map<std::string, std::shared_ptr<AudioTrack>> s_Tracks;
    };
}