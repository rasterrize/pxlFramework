#include "TestApplication.h"
#include <pxl.h>

namespace TestApp
{
    TestApplication::TestApplication()
    {
        pxl::Window::Init(1280, 720, "TestApp");
    }

    TestApplication::~TestApplication()
    {

    }
}