#include "EmptyApp.h"

#include <pxl/pxl.h>

#include <thread>

namespace TestApp
{
    void EmptyApp::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        APP_LOG_INFO("Running empty app...");

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}