#include "AudioTest.h"

namespace TestApp
{
    void AudioTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        pxl::AudioManager::Init();

        // Load audio tracks
        auto audioTrack1 = pxl::FileSystem::LoadAudioTrack("assets/audio/wings.mp3");
        auto audioTrack2 = pxl::FileSystem::LoadAudioTrack("assets/audio/oneway.flac");

        // Save them to the library so they don't immediately get freed
        pxl::AudioManager::Add("wings", audioTrack1);
        pxl::AudioManager::Add("oneway", audioTrack2);

        // Auto play sound
        pxl::AudioManager::Play("oneway");
    }

    void AudioTest::OnClose()
    {
        pxl::AudioManager::Shutdown();
    }

    void AudioTest::OnUpdate(float dt)
    {
    }

    void AudioTest::OnRender()
    {
    }

    void AudioTest::OnGUIRender()
    {
    }

    std::shared_ptr<pxl::Window> AudioTest::GetWindow()
    {
        return nullptr;
    }
}