#include "EmptyApp.h"

namespace TestApp
{
    void EmptyApp::OnStart(pxl::WindowSpecs& windowSpecs)
    {

    }

    void EmptyApp::OnUpdate(float dt)
    {
        PXL_PROFILE_SCOPE;
        
        pxl::Application::Get().Close();
        return;
    }

    void EmptyApp::OnRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void EmptyApp::OnGUIRender()
    {
        PXL_PROFILE_SCOPE;
    }

    void EmptyApp::OnClose()
    {

    }
}