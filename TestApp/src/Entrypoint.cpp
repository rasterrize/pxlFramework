#include "TestApplication.h"

int main()
{
    auto application = new TestApp::TestApplication();
    application->Run();
    delete application;
}