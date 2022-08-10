#include "Renderer.h"
#include "../../src/Core/Window.h"

namespace pxl
{
    bool Renderer::s_Enabled = false;
    RendererAPI Renderer::s_RendererAPI;
    GLFWwindow* Renderer::s_WindowHandle;

    void Renderer::Init(RendererAPI api)
    {   
        if (s_Enabled)
        {
            Logger::Log(LogLevel::Warn, "Can't initialize renderer, it's already initialized.");
        }

        s_WindowHandle = Window::GetNativeWindow();

        if (!s_WindowHandle)
        {
            Logger::Log(LogLevel::Error, "Can't initialize renderer, window must be initialized first");
        }

        // switch (api)
        // {
    
        // }

        s_Enabled = true;
        s_RendererAPI = api;

    }

    void Renderer::Shutdown()
    {
        s_Enabled = false;
    }

    void Renderer::Clear()
    {

    }
}