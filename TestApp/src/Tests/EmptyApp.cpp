#include "EmptyApp.h"

namespace TestApp
{
    void EmptyApp::OnStart(pxl::WindowSpecs& windowSpecs)
    {

    }

    void EmptyApp::OnUpdate(float dt)
    {
        pxl::Application::Get().Close();
        return;
    }

    void EmptyApp::OnRender()
    {

    }

    void EmptyApp::OnImGuiRender()
    {

    }

    void EmptyApp::OnClose()
    {

    }
}