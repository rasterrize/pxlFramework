#include "Window.h"

#include "Window/WindowGLFW.h"
#include "../Renderer/Renderer.h"

namespace pxl
{
    uint8_t Window::s_WindowCount = 0;
    int WindowGLFW::s_MonitorCount = 0;

    std::vector<std::shared_ptr<Window>> Window::s_Windows;

    Window::Window(const WindowSpecs& windowSpecs)
        : m_WindowSpecs(windowSpecs), m_WindowMode(WindowMode::Windowed)
    {
    }

    void Window::UpdateAll()
    {
        for (const auto& window : s_Windows)
        {
            window->Update();
        }

        Renderer::s_FrameCount++; // should be done at the end of making a frame in the renderer class (Renderer::EndFrame)
        Renderer::CalculateFPS();

        WindowGLFW::ProcessEvents(); // glfw docs use pollevents after swapbuffers // also this should be moved if I decide to support other platforms (linux/mac)
    }

    void Window::Shutdown()
    {
        uint32_t windowCount = s_WindowCount;

        for (uint32_t i = 0; i < windowCount; i++)
        {
            s_Windows[0]->Close();
        }
    }

    void Window::NextWindowMode()
    {
        switch(m_WindowMode)
        {
            case WindowMode::Windowed:
                SetWindowMode(WindowMode::Borderless);
                break;
            case WindowMode::Borderless:
                SetWindowMode(WindowMode::Fullscreen);
                break;
            case WindowMode::Fullscreen:
                SetWindowMode(WindowMode::Windowed);
                break;
        }
    }

    void Window::ToggleFullscreen()
    {
        if (m_WindowMode == WindowMode::Windowed || m_WindowMode == WindowMode::Borderless)
            SetWindowMode(WindowMode::Fullscreen);
        else if (m_WindowMode == WindowMode::Fullscreen)
            SetWindowMode(WindowMode::Windowed);
    }

    void Window::ToggleVSync()
    {
        if (!m_GraphicsContext->GetVSync())
            SetVSync(true);
        else
            SetVSync(false);
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        std::shared_ptr<Window> window;
        
        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                window = std::make_shared<WindowGLFW>(windowSpecs);
                break;
            case RendererAPIType::OpenGL:
                window = std::make_shared<WindowGLFW>(windowSpecs);
                break;
            case RendererAPIType::Vulkan:
                window = std::make_shared<WindowGLFW>(windowSpecs);
                break;
        }

        if (window)
        {
            window->m_Handle = window;
            s_Windows.push_back(window);
            return window;
        }

        return nullptr;
    }
}