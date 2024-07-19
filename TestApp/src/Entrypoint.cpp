#include "TestApplication.h"

#define MAIN_FUNC int main()

#ifdef TA_RELEASE
    #include <Windows.h>
    #define MAIN_FUNC int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

MAIN_FUNC
{
    // NOTE: Logging init function should be called before creating an application
    PXL_INIT_LOGGING;

    TestApp::TestApplication app;
    app.Run();
}