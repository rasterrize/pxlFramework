#ifndef TA_DEBUG
#include <Windows.h>
#endif

#include "TestApplication.h"

#ifdef TA_DEBUG
    int main()
#else
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    #ifdef TA_DEBUG
    pxl::Logger::Init();
    #endif
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}