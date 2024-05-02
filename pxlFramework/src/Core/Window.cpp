#include "Window.h"

#include "../Renderer/Renderer.h"
#include "Application.h"
#include "Input.h"
#include "../Debug/ImGui/pxl_ImGui.h"
#include "../Renderer/Vulkan/VulkanHelpers.h"
#include "../Renderer/Vulkan/VulkanContext.h"

namespace pxl
{
    constexpr uint8_t MAX_WINDOW_COUNT = 5; // TODO: currently unused, can be useful so the program can't accidently infinitely create windows

    uint8_t Window::s_WindowCount = 0;

    std::vector<Monitor> Window::s_Monitors;
    std::vector<std::shared_ptr<Window>> Window::s_Windows;

    std::function<void()> Window::s_EventProcessFunc = glfwPollEvents;

    Window::Window(const WindowSpecs& windowSpecs)
        : m_Specs(windowSpecs), m_LastWindowedWidth(m_Specs.Width), m_LastWindowedHeight(m_Specs.Height)
    {
        CreateGLFWWindow(windowSpecs);
        UpdateMonitors(); // safeguard for the moment
    }

    void Window::CreateGLFWWindow(const WindowSpecs& windowSpecs) // refresh rate/other params
    {
        glfwSetErrorCallback(GLFWErrorCallback); // Shouldn't be set everytime window is created
        
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
                    PXL_LOG_ERROR(LogArea::Window, "Vulkan loader wasn't found by GLFW");

                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
        }

        // Hide the window on creation so we can silently move the window to the center
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Create GLFW window and set it up
        m_GLFWWindow = glfwCreateWindow(static_cast<int>(windowSpecs.Width), static_cast<int>(windowSpecs.Height), windowSpecs.Title.c_str(), nullptr, nullptr);

        // Check to see if the window object was created successfully
        PXL_ASSERT(m_GLFWWindow);

        // Set window to the center of the display
        auto vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // TODO: allow the monitor to be specified be window creation.
        glfwSetWindowPos(m_GLFWWindow, vidMode->width / 2 - windowSpecs.Width / 2, vidMode->height / 2 - windowSpecs.Height / 2);

        s_WindowCount++;

        if (m_GLFWWindow)
            PXL_LOG_INFO(LogArea::Window, "Created GLFW window '{}' of size {}x{}", windowSpecs.Title, windowSpecs.Width, windowSpecs.Height)
        else
            PXL_LOG_ERROR(LogArea::Window, "Failed to create GLFW window '{}'", windowSpecs.Title)

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        SetStaticGLFWCallbacks();
        SetGLFWCallbacks();
    }

    void Window::Update() const
    {
        if (m_GraphicsContext) // this can be kept since it takes like less than a microsecond to check for this to evaluate.
            m_GraphicsContext->Present();
    }

    void Window::Close() const
    {
        glfwDestroyWindow(m_GLFWWindow);
        s_Windows.erase(std::find(s_Windows.begin(), s_Windows.end(), m_Handle.lock()));
        --s_WindowCount;

        if (pxl_ImGui::GetWindowHandle() == m_Handle.lock())
            pxl_ImGui::Shutdown();

        if (Renderer::GetGraphicsContext() == m_GraphicsContext) // this doesnt feel right
            Renderer::Shutdown();

        if (s_WindowCount == 0)
            Application::Get().Close();
    }

    void Window::SetGLFWCallbacks()
    {
        glfwSetWindowCloseCallback(m_GLFWWindow, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_GLFWWindow, WindowResizeCallback);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, FramebufferResizeCallback);
        glfwSetWindowIconifyCallback(m_GLFWWindow, WindowIconifyCallback);

        glfwSetKeyCallback(m_GLFWWindow, Input::GLFWKeyCallback);
        glfwSetMouseButtonCallback(m_GLFWWindow, Input::GLFWMouseButtonCallback);
        glfwSetScrollCallback(m_GLFWWindow, Input::GLFWScrollCallback);
        glfwSetCursorPosCallback(m_GLFWWindow, Input::GLFWCursorPosCallback);
    }

    void Window::SetStaticGLFWCallbacks()
    {
        glfwSetMonitorCallback(MonitorCallback);
    }

    GLFWmonitor* Window::GetWindowsCurrentGLFWMonitor()
    {
        int windowX, windowY;
        glfwGetWindowPos(m_GLFWWindow, &windowX, &windowY);

        int monitorX, monitorY;
        int monitorWidth, monitorHeight;

        for (int i = 0; i < s_Monitors.size(); i++)
        {   
            glfwGetMonitorWorkarea(s_Monitors[i].GLFWMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

            if ((windowX >= monitorX && windowX < (monitorX + monitorWidth)) && (windowY >= monitorY && windowY < (monitorY + monitorHeight)))
                return s_Monitors[i].GLFWMonitor;
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
    }

    void Window::SetWindowMode(WindowMode winMode)
    { 
        if (winMode == m_Specs.WindowMode)
            return;

        auto currentMonitor = GetWindowsCurrentGLFWMonitor();
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
        SetMonitor(s_Monitors[monitorIndex - 1]);
    }

    void Window::SetMonitor(const Monitor& monitor)
    {
        if (!monitor.GLFWMonitor)
        {
            PXL_LOG_WARN(LogArea::Window, "Can't set specified monitor for window '{}', Monitor doesn't exist", m_Specs.Title);
            return;
        }

        // Get the current video mode of the specified monitor
        auto vidmode = monitor.GetCurrentVideoMode();

        int nextMonX, nextMonY;
        glfwGetMonitorWorkarea(monitor.GLFWMonitor, &nextMonX, &nextMonY, NULL, NULL);

        switch (m_Specs.WindowMode)
        {
            case WindowMode::Windowed:
                SetPosition(nextMonX + (vidmode->width / 2) - (m_Specs.Width / 2), nextMonY + (vidmode->height / 2) - (m_Specs.Height / 2));
                break;
            case WindowMode::Borderless:
                SetPosition(nextMonX, nextMonY);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, monitor.GLFWMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
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

    void Window::SetVisibility(bool value)
    {
        value ? glfwShowWindow(m_GLFWWindow) : glfwHideWindow(m_GLFWWindow);
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
        fb.x = static_cast<uint32_t>(width);
        fb.y = static_cast<uint32_t>(height);
        
        return fb;
    }

    VkSurfaceKHR Window::CreateVKWindowSurface(VkInstance instance)
    {
        // Create VkSurfaceKHR for glfw window
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VK_CHECK(glfwCreateWindowSurface(instance, m_GLFWWindow, nullptr, &surface)); // could learn to do this myself https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface

        PXL_ASSERT(surface)

        return surface;
    }

    void Window::GLFWErrorCallback(int error, const char* description)
    {
        PXL_LOG_ERROR(LogArea::Window, "GLFW ERROR: {} - {}", error, description);
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->Close();
    }

    void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        windowInstance->m_Specs.Width = width;
        windowInstance->m_Specs.Height = height;
    }

    void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (Renderer::IsInitialized())
        {
            Renderer::ResizeViewport(width, height);
            Renderer::ResizeScissor(width, height);
        }

        if (width == 0 && height == 0)
            return;

        if (Renderer::GetCurrentAPI() == RendererAPIType::Vulkan)
        {
            auto vulkanContext = std::dynamic_pointer_cast<VulkanGraphicsContext>(windowInstance->GetGraphicsContext());

            if (vulkanContext)
                vulkanContext->GetSwapchain()->SetExtent( { static_cast<uint32_t>(width), static_cast<uint32_t>(height) } ); // might need other data  
        }
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        // probably won't work for multiple windows
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        iconified ? windowInstance->m_Specs.Minimized = true : windowInstance->m_Specs.Minimized = false;
        
        // Check if all windows are minimized
        bool allWindowsIconified = true;
        for (const auto& windowHandle : s_Windows)
        {
            if (windowHandle->m_Specs.Minimized == false)
                allWindowsIconified = false;
        }

        if (allWindowsIconified)
        {
            Application::Get().SetMinimization(true);
            s_EventProcessFunc = glfwWaitEvents;
        }
        else
        {
            Application::Get().SetMinimization(false);
            s_EventProcessFunc = glfwPollEvents;
        }
    }

    void Window::MonitorCallback(GLFWmonitor* monitor, int event)
    {
        if (event == GLFW_CONNECTED || event == GLFW_DISCONNECTED) // Using this we could dynamically add and remove the specified monitor from s_Monitors.
        {
            // NOTE: Either GLFW or the OS is handling moving the window when a monitor is disconnected. This might break stuff and needs extra testing
        }

        UpdateMonitors();
    }

    void Window::UpdateAll()
    {
        for (const auto& window : s_Windows)
            window->Update();

        Renderer::s_FrameCount++; // should be done at the end of making a frame in the renderer class (Renderer::EndFrame)
        Renderer::CalculateFPS();

        s_EventProcessFunc(); // glfw docs use pollevents after swapbuffers // also this should be moved if I decide to support other platforms (linux/mac)
    }

    void Window::UpdateMonitors()
    {
        int monitorCount;
        auto glfwMonitors = glfwGetMonitors(&monitorCount);

        for (int i = 0; i < monitorCount; i++)
        {
            Monitor monitor;
            monitor.Name = glfwGetMonitorName(glfwMonitors[i]);
            monitor.Index = i + 1;
            monitor.GLFWMonitor = glfwMonitors[i];

            int vidModeCount = 0;
            auto vidModes = glfwGetVideoModes(glfwMonitors[i], &vidModeCount);

            monitor.VideoModes.insert(monitor.VideoModes.end(), &vidModes[0], &vidModes[vidModeCount]);

            monitor.IsPrimary = monitor.GLFWMonitor == glfwGetPrimaryMonitor() ? true : false;

            s_Monitors.push_back(monitor);
        }
    }

    void Window::Shutdown()
    {
        uint8_t windowCount = s_WindowCount; // s_WindowCount changes each time a window is closed

        for (uint8_t i = 0; i < windowCount; i++)
            s_Windows[i]->Close();

        // TODO: check if GLFW is in use by other systems first before terminating it.
        glfwTerminate();
        
        PXL_LOG_INFO(LogArea::Window, "GLFW terminated")
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        if (s_WindowCount >= MAX_WINDOW_COUNT)
        {
            PXL_LOG_WARN(LogArea::Window, "Failed to create window, the max window count has been reached")
            return nullptr;
        }
        
        auto window = std::make_shared<Window>(windowSpecs);

        PXL_ASSERT(window);

        if (window)
        {
            window->m_Handle = window;
            if (windowSpecs.RendererAPI != RendererAPIType::None)
            {
                window->m_GraphicsContext = GraphicsContext::Create(windowSpecs.RendererAPI, window); // Automatically create a graphics context for the window

                PXL_ASSERT(window->m_GraphicsContext != nullptr);
            }

            // Set the visibility now since we have a valid context
            window->SetVisibility(true);

            s_Windows.push_back(window);
            return window;
        }

        return nullptr;
    }
}