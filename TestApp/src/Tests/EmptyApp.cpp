#include "EmptyApp.h"

#include <thread>

namespace TestApp
{
    void EmptyApp::OnStart(pxl::WindowSpecs& windowSpecs)
    {
    }

    void EmptyApp::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    void EmptyApp::OnRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void EmptyApp::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void EmptyApp::OnClose()
    {
    }
}