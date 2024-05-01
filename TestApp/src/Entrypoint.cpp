#include "TestApplication.h"

#ifdef TA_DIST
    #include <Windows.h>
    #define MAIN_FUNC int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    #define MAIN_FUNC int main()
#endif

MAIN_FUNC
{
    // Init log before creating application
    #ifndef PXL_DISABLE_LOGGING
        pxl::Logger::Init();
    #endif

    TestApp::TestApplication application; // whether this should be on the heap or stack depends on big this class might get.
    application.Run();
}