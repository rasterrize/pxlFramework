#include "TestApplication.h"

#if defined(TA_RELEASE) && defined(_WIN32)
    #define MAIN_FUNC() int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    #define TA_USING_WINMAIN 1
#else
    #define MAIN_FUNC() int main(int argc, char* argv[])
    #define TA_USING_WINMAIN 0
#endif

MAIN_FUNC()
{
    PXL_INIT_LOGGING;

    std::vector<std::string> launchArgs;
#if TA_USING_WINMAIN
    launchArgs.push_back(std::string(lpCmdLine));
#else
    for (int i = 0; i < argc; i++)
        launchArgs.push_back(std::string(argv[i]));
#endif

    TestApp::TestApplication app(launchArgs);

    app.Run();
}