#ifdef PXL_RELEASE
#include <Windows.h>
#endif

#include "TestApplication.h"

#ifdef PXL_RELEASE
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // GLFW defines this as well (probably unavoidable)
#else
    int main()
#endif
{
    pxl::Logger::Init();
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}