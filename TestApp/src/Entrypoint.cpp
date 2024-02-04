#ifdef TA_RELEASE
    #include <Windows.h>
#endif

#include "TestApplication.h"

#ifdef TA_RELEASE
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    int main()
#endif
{
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}