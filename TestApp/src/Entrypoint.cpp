#include "TestApplication.h"

int main()
{
    pxl::Logger::Init();
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}