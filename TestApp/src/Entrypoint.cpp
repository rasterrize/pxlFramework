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
    #ifdef PXL_ENABLE_LOGGING
        pxl::Logger::Init();
    #endif

    std::unique_ptr<TestApp::TestApplication> app = std::make_unique<TestApp::TestApplication>();
    app->Run();
}