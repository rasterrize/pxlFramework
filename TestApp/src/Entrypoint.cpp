#include "TestApplication.h"

#ifdef TA_DIST
    #include <Windows.h>
#endif

#ifndef TA_DIST
    int main()
#else
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    #ifndef TA_DIST
        pxl::Logger::Init();
    #endif
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}