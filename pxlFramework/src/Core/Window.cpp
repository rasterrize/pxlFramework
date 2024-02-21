#include "Window.h"

#include "../Renderer/Renderer.h"
#include "Application.h"

#include "Input.h"
#include "../Debug/ImGui/pxl_ImGui.h"

#include <vulkan/vulkan.h>
#include "../Renderer/Vulkan/VulkanHelpers.h"

namespace pxl
{
    constexpr uint8_t MAX_WINDOW_COUNT = 5; // TODO: currently unused, can be useful so the program cant accidently infinitely create windows

    uint8_t Window::s_WindowCount = 0;
    uint8_t Window::s_MonitorCount = 0;

    GLFWmonitor** Window::s_Monitors;
    std::vector<std::shared_ptr<Window>> Window::s_Windows;

    bool Window::s_AllWindowsMinimized = false; // true at start?

    Window::Window(const WindowSpecs& windowSpecs)
        : m_Specs(windowSpecs), m_LastWindowedWidth(m_Specs.Width), m_LastWindowedHeight(m_Specs.Height)
    {
        CreateGLFWWindow(windowSpecs);
        s_WindowCount++;
    }

    void Window::CreateGLFWWindow(const WindowSpecs& windowSpecs) // refresh rate/other params
    {
        if (s_WindowCount == 0)
        {
            if (glfwInit())
            {
                int major = 0, minor = 0, rev = 0;
                glfwGetVersion(&major, &minor, &rev);
                PXL_LOG_INFO(LogArea::Window, "GLFW initialized - Version {}.{}.{}", major, minor, rev);
            }
            else
            {
                PXL_LOG_ERROR(LogArea::Window, "Failed to initialize GLFW");
                // TODO: glfw error callback and print those errors
                return;
            }
        }

        // reset window hints so we don't get undefined behaviour
        glfwDefaultWindowHints();

        // Set window hints based on renderer api
        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                PXL_LOG_WARN(LogArea::Window, "RendererAPI type 'none' specified! Creating a GLFW window with no renderer api...");
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            case RendererAPIType::OpenGL:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                break;
            case RendererAPIType::Vulkan:
                if (!glfwVulkanSupported())
                {
                    PXL_LOG_WARN(LogArea::Window, "Vulkan loader wasn't found by GLFW");
                }

                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
        }

        // Create GLFW window and set it up
        m_GLFWWindow = glfwCreateWindow((int)windowSpecs.Width, (int)windowSpecs.Height, windowSpecs.Title.c_str(), NULL, NULL);
        glfwSetWindowUserPointer(m_GLFWWindow, (Window*)this);

        GetGLFWMonitors();
        SetGLFWCallbacks();

        // Check to see if the window object was created successfully
        if (m_GLFWWindow)
        {
            PXL_LOG_INFO(LogArea::Window, "Created GLFW window '{}' of size {}x{}", windowSpecs.Title, windowSpecs.Width, windowSpecs.Height);
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Window, "Failed to create GLFW window '{}'", windowSpecs.Title);
            if (s_WindowCount == 0)
                glfwTerminate();
        }
    }

    void Window::Update()
    {
        if (m_GraphicsContext)
            m_GraphicsContext->Present();
    }

    void Window::Close()
    {
        glfwDestroyWindow(m_GLFWWindow);
        s_Windows.erase(std::find(s_Windows.begin(), s_Windows.end(), m_Handle));
        --s_WindowCount;

        if (pxl_ImGui::GetWindowHandle() == m_Handle)
            pxl_ImGui::Shutdown();

        if (Renderer::GetWindowHandle() == m_Handle) // this doesnt feel right
            Renderer::Shutdown();

        if (s_WindowCount == 0)
        {
            glfwTerminate(); // TODO: shouldnt terminate glfw since it might be still in use by another system (eg audio)
            Application::Get().Close();
        }
    }

    void Window::SetGLFWCallbacks()
    {
        glfwSetWindowCloseCallback(m_GLFWWindow, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_GLFWWindow, WindowResizeCallback);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, FramebufferResizeCallback);
        glfwSetWindowIconifyCallback(m_GLFWWindow, WindowIconifyCallback);

        glfwSetMonitorCallback(MonitorCallback);

        glfwSetKeyCallback(m_GLFWWindow, Input::GLFWKeyCallback);
        glfwSetMouseButtonCallback(m_GLFWWindow, Input::GLFWMouseButtonCallback);
        glfwSetScrollCallback(m_GLFWWindow, Input::GLFWScrollCallback);
        glfwSetCursorPosCallback(m_GLFWWindow, Input::GLFWCursorPosCallback);
    }

    GLFWmonitor* Window::GetCurrentMonitor()
    {
        int windowX, windowY;
        glfwGetWindowPos(m_GLFWWindow, &windowX, &windowY);

        int monitorX, monitorY;
        int monitorWidth, monitorHeight;

        for (int i = 0; i < s_MonitorCount; i++)
        {   
            glfwGetMonitorWorkarea(s_Monitors[i], &monitorX, &monitorY, &monitorWidth, &monitorHeight);
            if ((windowX >= monitorX && windowX < (monitorX + monitorWidth)) && (windowY >= monitorY && windowY < (monitorY + monitorHeight)))
            {
                return s_Monitors[i];
            }
        }

        PXL_LOG_ERROR(LogArea::Window, "Failed to get window current monitor");
        return nullptr;
    }

    void Window::SetSize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(m_GLFWWindow, width, height);

        // Check for successful window size change
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_GLFWWindow, &windowWidth, &windowHeight);
        
        if (windowWidth != width | windowHeight != height)
            PXL_LOG_WARN(LogArea::Window, "Failed to change window '{}' resolution to {}x{}", m_Specs.Title, width, height);
    }

    void Window::SetPosition(uint32_t xpos, uint32_t ypos)
    {
        // TODO: check if the position is inbounds and maybe check monitor properties to correctly set things

        glfwSetWindowPos(m_GLFWWindow, xpos, ypos);
        // TODO: add logging
    }

    void Window::SetWindowMode(WindowMode winMode)
    { 
        if (winMode == m_Specs.WindowMode)
            return;

        auto currentMonitor = GetCurrentMonitor();
        if (!currentMonitor)
        {
            currentMonitor = glfwGetPrimaryMonitor();
            PXL_LOG_WARN(LogArea::Window, "Current monitor was null, so the primary monitor was used");
        }

        const GLFWvidmode* vidmode = glfwGetVideoMode(currentMonitor);

        int monitorX, monitorY;
        int monitorWidth, monitorHeight;
        glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

        switch (winMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_TRUE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX + (vidmode->width / 2) - (m_LastWindowedWidth / 2), monitorY + (vidmode->height / 2) - (m_LastWindowedHeight / 2), m_LastWindowedWidth, m_LastWindowedHeight, GLFW_DONT_CARE); // TODO: store the windowed window size so it can be restored instead of fixed 1280x720
                m_Specs.WindowMode = WindowMode::Windowed;
                PXL_LOG_INFO(LogArea::Window, "Switched '{}' to Windowed window mode", m_Specs.Title);
                break;
            case WindowMode::Borderless:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_FALSE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX, monitorY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                m_Specs.WindowMode = WindowMode::Borderless;
                PXL_LOG_INFO(LogArea::Window, "Switched '{}' to Borderless window mode", m_Specs.Title);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, currentMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                m_GraphicsContext->SetVSync(m_GraphicsContext->GetVSync()); // Set VSync because bug idk // TODO: this needs to be tested with vulkan
                m_Specs.WindowMode = WindowMode::Fullscreen;
                PXL_LOG_INFO(LogArea::Window, "Switched '{}' to Fullscreen window mode", m_Specs.Title);
                break;
        }
    }

    void Window::SetMonitor(uint8_t monitorIndex)
    {
        if (monitorIndex > s_MonitorCount || monitorIndex <= 0)
        {
            PXL_LOG_WARN(LogArea::Window, "Can't set specified monitor for window '{}', Monitor doesn't exist", m_Specs.Title);
            return;
        }

        // Get the current video mode of the specified monitor
        GLFWmonitor* monitor = s_Monitors[monitorIndex - 1]; // monitor indexes start at 1, arrays start at 0
        const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

        int nextMonX, nextMonY;
        glfwGetMonitorWorkarea(monitor, &nextMonX, &nextMonY, NULL, NULL);

        switch (m_Specs.WindowMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, nextMonX + (vidmode->width / 2) - (m_Specs.Width / 2), nextMonY + (vidmode->height / 2) - (m_Specs.Height / 2), m_Specs.Width, m_Specs.Height, GLFW_DONT_CARE);
                break;
            case WindowMode::Borderless:
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, nextMonX, nextMonY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                break;
        }
    }

    void Window::NextWindowMode()
    {
        switch(m_Specs.WindowMode)
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
        if (m_Specs.WindowMode == WindowMode::Windowed || m_Specs.WindowMode == WindowMode::Borderless)
            SetWindowMode(WindowMode::Fullscreen);
        else if (m_Specs.WindowMode == WindowMode::Fullscreen)
            SetWindowMode(WindowMode::Windowed);
    }

    void Window::ToggleVSync()
    {
        if (!m_GraphicsContext->GetVSync())
            SetVSync(true);
        else
            SetVSync(false);
    }

    void Window::SetVisibility(bool value)
    {   
        if (value)
            glfwShowWindow(m_GLFWWindow);
        else
            glfwHideWindow(m_GLFWWindow);
    }

    std::vector<const char*> Window::GetVKRequiredInstanceExtensions()
    {
        uint32_t glfwExtensionCount = 0;
	    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	    return { glfwExtensions, glfwExtensions + glfwExtensionCount }; // need to research how this works lol
    }

    glm::u32vec2 Window::GetFramebufferSize()
    {
        int width, height;
        glfwGetFramebufferSize(m_GLFWWindow, &width, &height);

        glm::u32vec2 fb;
        fb.x = (uint32_t)width;
        fb.y = (uint32_t)height;
        
        return fb;
    }

    VkSurfaceKHR Window::CreateVKWindowSurface(VkInstance instance)
    {
        // Create VkSurfaceKHR for glfw window
        VkSurfaceKHR surface;
        VkResult result = glfwCreateWindowSurface(instance, m_GLFWWindow, nullptr, &surface); // could learn to do this myself https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface
        VulkanHelpers::CheckVkResult(result);

        if (!surface)
        {
            PXL_LOG_ERROR(LogArea::Window, "Failed to create window surface");
            return VK_NULL_HANDLE;
        }

        return surface;
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        auto windowHandle = (Window*)glfwGetWindowUserPointer(window);
        windowHandle->Close();
    }

    void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight); // should be GetFramebufferSize();

        auto windowInstance = (Window*)glfwGetWindowUserPointer(window);
        windowInstance->m_Specs.Width = width;
        windowInstance->m_Specs.Height = height;
        Renderer::ResizeViewport(fbWidth, fbHeight); // idk if GraphicsContext should be doing this but this is currently necessary for opengl
        Renderer::ResizeScissor(fbWidth, fbHeight);
    }

    void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto windowInstance = (Window*)glfwGetWindowUserPointer(window);
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        // probably won't work for multiple windows
        auto windowInstance = (Window*)glfwGetWindowUserPointer(window);

        if (iconified)
        {
            windowInstance->m_Specs.Minimized = true;
        }
        else
        {
            windowInstance->m_Specs.Minimized = false;
        }
        
        // Check if all windows are minimized
        bool allWindowsIconified = true;
        for (const auto& window : s_Windows)
        {
            if (window->m_Specs.Minimized == false)
                allWindowsIconified = false;
        }

        if (allWindowsIconified)
        {
            s_AllWindowsMinimized = true;
            Application::Get().SetMinimization(true);
        }
        else
        {
            s_AllWindowsMinimized = false;
            Application::Get().SetMinimization(false);
        }
    }

    void Window::MonitorCallback(GLFWmonitor* monitor, int event)
    {
        if (event == GLFW_CONNECTED || event == GLFW_DISCONNECTED)
        {
            GetGLFWMonitors();
            // TODO: handle windows on disconnected monitors?
        }
    }

    void Window::GetGLFWMonitors()
    {
        int monitorCount;
        s_Monitors = glfwGetMonitors(&monitorCount);
        s_MonitorCount = (uint8_t)monitorCount;
    }

    void Window::UpdateAll()
    {
        for (const auto& window : s_Windows)
        {
            window->Update();
        }

        Renderer::s_FrameCount++; // should be done at the end of making a frame in the renderer class (Renderer::EndFrame)
        Renderer::CalculateFPS();

        if (!s_AllWindowsMinimized)
            Window::PollEvents(); // glfw docs use pollevents after swapbuffers // also this should be moved if I decide to support other platforms (linux/mac)
        else
            Window::WaitEvents();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::WaitEvents()
    {
        glfwWaitEvents();
    }

    void Window::Shutdown()
    {
        uint8_t windowCount = s_WindowCount; // s_WindowCount changes each time a window is closed

        for (uint8_t i = 0; i < windowCount; i++)
        {
            s_Windows[i]->Close();
        }
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        auto window = std::make_shared<Window>(windowSpecs);

        if (window)
        {
            window->m_Handle = window;
            if (windowSpecs.RendererAPI != RendererAPIType::None)
            {
                window->m_GraphicsContext = GraphicsContext::Create(windowSpecs.RendererAPI, window); // Automatically create a graphics context for the window

                if (!window->m_GraphicsContext)
                {
                    PXL_LOG_ERROR(LogArea::Window, "Failed to create graphics context for window {}", windowSpecs.Title);
                }
            }

            s_Windows.push_back(window);
            return window;
        }

        return nullptr;
    }
}