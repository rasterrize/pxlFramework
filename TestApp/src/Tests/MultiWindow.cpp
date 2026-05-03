#include "MultiWindow.h"

namespace TestApp
{
    void MultiWindow::OnStart(pxl::WindowSpecs& windowSpecs)
    {
        pxl::WindowSpecs customSpecs = {};
        customSpecs.Title = "pxlFramework Test App - Window Test - Window A";
        customSpecs.WindowMode = pxl::WindowMode::Windowed;
        m_TestWindowA = pxl::Window::Create(customSpecs);

        customSpecs.Title = "pxlFramework Test App - Window Test - Window B";
        customSpecs.Size = { 800, 600 };
        m_TestWindowB = pxl::Window::Create(customSpecs);
    }

    void MultiWindow::OnUpdate(float dt)
    {
    }
}