#include "Window.h"

#include "Renderer/Renderer.h"
#include "Application.h"
#include "Input.h"
#include "Renderer/Vulkan/VulkanHelpers.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanInstance.h"

namespace pxl
{
    static constexpr uint8_t MAX_WINDOW_COUNT = 5; // TODO: Use different naming convention

    Window::Window(const WindowSpecs& windowSpecs)
        : m_Specs(windowSpecs), m_LastWindowedWidth(m_Specs.Width), m_LastWindowedHeight(m_Specs.Height)
    {
        CreateGLFWWindow(windowSpecs);
        UpdateMonitors(); // NOTE: Safeguard, so the monitors are updated on first window creation
    }

    void Window::CreateGLFWWindow(const WindowSpecs& windowSpecs)
    {
        glfwSetErrorCallback(GLFWErrorCallback); // Shouldn't be set everytime window is created
        
        if (s_Windows.empty())
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

        // Reset window hints so we don't get undefined behaviour
        glfwDefaultWindowHints();

        // Hide the window on creation as we will still need to prepare it
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Set window hints based on renderer api
        switch (windowSpecs.RendererAPI)
        {
            case RendererAPIType::None:
                PXL_LOG_WARN(LogArea::Window, "RendererAPI type 'None' specified! Creating a GLFW window with no renderer api...");
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            case RendererAPIType::OpenGL:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                break;
            case RendererAPIType::Vulkan:
                PXL_ASSERT_MSG(glfwVulkanSupported(), "Vulkan loader wasn't found by GLFW");

                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
        }

        m_GLFWWindow = glfwCreateWindow(static_cast<int>(windowSpecs.Width), static_cast<int>(windowSpecs.Height), windowSpecs.Title.c_str(), nullptr, nullptr);

        PXL_ASSERT_MSG(m_GLFWWindow, "Failed to create GLFW window '{}'", windowSpecs.Title);

        PXL_LOG_INFO(LogArea::Window, "Created GLFW window '{}' of size {}x{}", windowSpecs.Title, windowSpecs.Width, windowSpecs.Height)
        
        // Set window to the center of the display
        auto vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // TODO: allow the monitor to be specified be window creation.
        glfwSetWindowPos(m_GLFWWindow, vidMode->width / 2 - windowSpecs.Width / 2, vidMode->height / 2 - windowSpecs.Height / 2);

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        SetStaticGLFWCallbacks();
        SetGLFWCallbacks();
    }

    void Window::Update() const
    {
        PXL_PROFILE_SCOPE;
        
        if (m_GraphicsContext)
            m_GraphicsContext->Present();
    }

    void Window::Close() const
    {
        glfwDestroyWindow(m_GLFWWindow);
        s_Windows.erase(std::find(s_Windows.begin(), s_Windows.end(), m_Handle.lock()));

        if (s_Windows.empty() && Application::Get().IsRunning())
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
        
        PXL_ASSERT_MSG(windowWidth != static_cast<int>(width) || windowHeight != static_cast<int>(height), "Failed to change window '{}' resolution to {}x{}", m_Specs.Title, width, height);
    }

    void Window::SetPosition(uint32_t xpos, uint32_t ypos)
    {
        // TODO: check if the position is inbounds and maybe check monitor properties before setting windows pos

        glfwSetWindowPos(m_GLFWWindow, xpos, ypos);

        PXL_LOG_INFO(LogArea::Window, "Manually set window position to {}, {}", xpos, ypos);
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

        int monitorX, monitorY, monitorWidth, monitorHeight;
        glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

        switch (winMode)
        {
            case WindowMode::Windowed:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_TRUE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX + (vidmode->width / 2) - (m_LastWindowedWidth / 2), monitorY + (vidmode->height / 2) - (m_LastWindowedHeight / 2), m_LastWindowedWidth, m_LastWindowedHeight, GLFW_DONT_CARE); // TODO: store the windowed window size so it can be restored instead of fixed 1280x720
                m_Specs.WindowMode = WindowMode::Windowed;
                PXL_LOG_INFO(LogArea::Window, "Switched '{}' to Windowed window mode", m_Specs.Title);
                return;
            case WindowMode::Borderless:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_FALSE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX, monitorY, vidmode->width, vidmode->height, GLFW_DONT_CARE);
                m_Specs.WindowMode = WindowMode::Borderless;
                PXL_LOG_INFO(LogArea::Window, "Switched '{}' to Borderless window mode", m_Specs.Title);
                return;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, currentMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
                m_Specs.WindowMode = WindowMode::Fullscreen;
                PXL_LOG_INFO(LogArea::Window, "Switched '{}' to Fullscreen window mode", m_Specs.Title);
                return;
        }

        PXL_LOG_WARN(LogArea::Window, "Failed to switch window mode for window '{}'", m_Specs.Title);
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
                return;
            case WindowMode::Borderless:
                SetWindowMode(WindowMode::Fullscreen);
                return;
            case WindowMode::Fullscreen:
                SetWindowMode(WindowMode::Windowed);
                return;
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

    const Monitor& Window::GetPrimaryMonitor()
    {
        for (const auto& monitor : s_Monitors)
        {
            if (monitor.IsPrimary)  
                return monitor;
        }

        PXL_LOG_WARN(LogArea::Window, "Failed to find primary monitor");

        return s_Monitors[0];
    }

    std::vector<const char *> Window::GetVKRequiredInstanceExtensions()
    {
        uint32_t glfwExtensionCount = 0;
	    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	    return { glfwExtensions, glfwExtensions + glfwExtensionCount }; // need to research how this works lol
    }

    Size2D Window::GetFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_GLFWWindow, &width, &height);
        
        return Size2D(width, height);
    }

    VkSurfaceKHR Window::CreateVKWindowSurface(VkInstance instance)
    {
        // Create VkSurfaceKHR for glfw window
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VK_CHECK(glfwCreateWindowSurface(instance, m_GLFWWindow, nullptr, &surface)); // could learn to do this myself https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface

        PXL_ASSERT(surface)

        return surface;
    }

    void Window::GLFWErrorCallback([[maybe_unused]] int error, [[maybe_unused]] const char* description)
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
            Renderer::ResizeViewport(0, 0, width, height);
            Renderer::ResizeScissor(0, 0, width, height);
        }

        if (Renderer::GetCurrentAPI() == RendererAPIType::Vulkan)
        {
            auto swapchain = std::dynamic_pointer_cast<VulkanGraphicsContext>(windowInstance->m_GraphicsContext)->GetSwapchain();
            auto swapchainSpecs = swapchain->GetSwapchainSpecs();

            if (width == 0 && height == 0)
            {
                swapchain->Suspend();
                return;
            }

            swapchain->Continue();

            if (static_cast<int32_t>(swapchainSpecs.Extent.width) != width || static_cast<int32_t>(swapchainSpecs.Extent.height) != height)
                swapchain->Recreate(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
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

    void Window::MonitorCallback([[maybe_unused]] GLFWmonitor* monitor, int event)
    {
        if (event == GLFW_CONNECTED || event == GLFW_DISCONNECTED)
        {
            // TODO: Dynamically add and remove the specified monitor from s_Monitors
        }

        UpdateMonitors();
    }

    void Window::UpdateAll()
    {
        PXL_PROFILE_SCOPE;

        if (s_Windows.empty()) return;
        
        for (const auto& window : s_Windows)
            window->Update();

        s_EventProcessFunc(); // NOTE: this must be done second (on vulkan atleast) otherwise vulkan breaks (need to look into this)
    }

    void Window::UpdateMonitors()
    {
        int monitorCount;
        auto glfwMonitors = glfwGetMonitors(&monitorCount);

        for (uint8_t i = 0; i < monitorCount; i++)
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

    void Window::CloseAll()
    {
        auto windowCount = s_Windows.size();

        /* Window::Close() removes itself from the vector so we must
           always access the first element */
        for (size_t i = 0; i < windowCount; i++)
            s_Windows.front()->Close();
    }

    void Window::Shutdown()
    {
        CloseAll();
        
        // TODO: Check if GLFW is in use by other systems first before terminating it.
        glfwTerminate();
        
        PXL_LOG_INFO(LogArea::Window, "GLFW terminated");
        PXL_LOG_INFO(LogArea::Window, "Window system shutdown");
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        if (s_Windows.size() >= MAX_WINDOW_COUNT)
        {
            PXL_LOG_WARN(LogArea::Window, "Failed to create window, the max window count has been reached");
            return nullptr;
        }
        
        auto window = std::make_shared<Window>(windowSpecs);

        PXL_ASSERT(window);

        window->m_Handle = window;

        if (windowSpecs.RendererAPI != RendererAPIType::None)
        {
            if (windowSpecs.RendererAPI == RendererAPIType::Vulkan)
            {
                // Initialize the Vulkan instance if necessary
                if (!VulkanInstance::Get())
                {
                    std::vector<const char*> selectedExtensions;
                    std::vector<const char*> selectedLayers;

                    // We are only using the required extensions by glfw for now
                    // Should retrieve VK_KHR_SURFACE and platform specific extensions (VK_KHR_win32_SURFACE)
                    selectedExtensions = Window::GetVKRequiredInstanceExtensions();

                    // Get available instance layers
                    auto availableLayers = VulkanHelpers::GetAvailableInstanceLayers();

                    #ifdef PXL_DEBUG
                        // Get validation layer (Vulkan debugging)
                        selectedLayers.push_back(VulkanHelpers::GetValidationLayer(availableLayers));
                    #endif

                    VulkanInstance::Init(selectedExtensions, selectedLayers);
                }
            }

            // Automatically create a graphics context for the window
            window->m_GraphicsContext = GraphicsContext::Create(windowSpecs.RendererAPI, window);

            PXL_ASSERT_MSG(window->m_GraphicsContext, "Failed to create graphics context for window '{}'", window->GetWindowSpecs().Title);
        }

        // Set the visibility now since we have a valid context
        window->SetVisibility(true);

        s_Windows.push_back(window);
        return window;
    }
}