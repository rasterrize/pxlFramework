#include "AudioTest.h"

namespace TestApp
{
    void AudioTest::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        pxl::AudioManager::Init(nullptr);
    }

    void AudioTest::OnClose()
    {
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