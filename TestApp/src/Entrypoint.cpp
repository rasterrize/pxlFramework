#include "TestApplication.h"
#include "Tests/CubesTest.h"
#include "Tests/EmptyApp.h"
#include "Tests/LinesTest.h"
#include "Tests/ModelViewer.h"
#include "Tests/MultiWindow.h"
#include "Tests/OGLVK.h"
#include "Tests/QuadsTest.h"

#if defined(TA_RELEASE) && defined(_WIN64)
    #define MAIN_FUNC() int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
    #define MAIN_FUNC() int main(int argc, char* argv[])
#endif

MAIN_FUNC()
{
    PXL_INIT_LOGGING;

    TestApp::TestApplication app;

#ifndef TA_RELEASE
    // Parse launch arguments
    std::vector<std::string> launchArgs;

    // NOTE: Purposefully skips the first argument (the program's name/path)
    for (int i = 1; i < argc; i++)
        launchArgs.emplace_back(argv[i]);

    for (auto& string : launchArgs)
    {
        // Check if this is a valid argument
        if (string.front() != '-')
            continue;

        if (string.find("test", 0) != -1)
        {
            auto testValue = string.substr(6, string.length());

            // TODO: simplify this
            if (testValue == "ModelViewer")
                app.LaunchTest<TestApp::ModelViewer>();
            else if (testValue == "QuadsTest")
                app.LaunchTest<TestApp::QuadsTest>();
            else if (testValue == "CubesTest")
                app.LaunchTest<TestApp::CubesTest>();
            else if (testValue == "LinesTest")
                app.LaunchTest<TestApp::LinesTest>();
            else if (testValue == "EmptyApp")
                app.LaunchTest<TestApp::EmptyApp>();
            else if (testValue == "OGLVK")
                app.LaunchTest<TestApp::OGLVK>();
            else if (testValue == "MultiWindow")
                app.LaunchTest<TestApp::MultiWindow>();
        }
    }
#endif

    if (!app.HasTest())
        app.LaunchTest<TestApp::ModelViewer>();

    app.Run();
}