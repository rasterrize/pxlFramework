#include "TestApplication.h"

#ifdef TA_DIST
    #include <Windows.h>
#endif

#ifdef TA_DIST
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    int main()
#endif
{
    #ifndef TA_DIST
        pxl::Logger::Init();
    #endif
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}