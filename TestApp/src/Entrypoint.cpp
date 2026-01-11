#include "TestApplication.h"
#include "Tests/CubesTest.h"
#include "Tests/EmptyApp.h"
#include "Tests/LinesTest.h"
#include "Tests/ModelViewer.h"
#include "Tests/MultiWindow.h"
#include "Tests/OGLVK.h"
#include "Tests/QuadsTest.h"

#ifndef TA_RELEASE
    #define MAIN_FUNC() int main()
#else
    #define MAIN_FUNC() int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif

MAIN_FUNC()
{
    PXL_INIT_LOGGING;

    TestApp::TestApplication app;
    app.LaunchTest<TestApp::ModelViewer>();
    app.Run();
}