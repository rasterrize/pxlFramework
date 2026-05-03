#include "EmptyApp.h"

#include <pxl/pxl.h>

#include <thread>

#include "../TestApplication.h"

namespace TestApp
{
    void EmptyApp::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;

        APP_LOG_INFO("Running empty app...");

        std::this_thread::sleep_for(std::chrono::seconds(1));

        m_UpdateCount++;

        if (m_UpdateCount >= m_MaxUpdateCount)
        {
            TestApplication::Get().Close();
            return;
        }
    }
}