#include "TestApplication.h"

#ifdef TA_RELEASE
    #include <Windows.h>
    #define MAIN_FUNC int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    #define MAIN_FUNC int main()
#endif

MAIN_FUNC
{
    // Init log before creating application
    PXL_INIT_LOGGING;

    TestApp::TestApplication app;
    app.Run();
}