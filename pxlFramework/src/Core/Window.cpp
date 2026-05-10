#include "Window.h"

#include "Application.h"
#include "Events/WindowEvents.h"
#include "Renderer/Renderer.h"
#include "Renderer/Vulkan/VulkanUtils.h"
#include "Utils/FileSystem.h"

#ifdef _WIN32
    #include "Platform/Windows/WindowsUtils.h"
#endif

namespace pxl
{
    Window::Window(const WindowSpecs& specs)
        : m_Size(specs.WindowedSize), m_WindowMode(specs.WindowMode)
    {
        // Reset window hints so we don't get irregular behaviour
        glfwDefaultWindowHints();

        // Disable auto iconification, we will handle this ourselves
        glfwWindowHint(GLFW_AUTO_ICONIFY, false);

        // GLFW uses OpenGL by default, which we don't want to use
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        /* Hide the window on creation as we will still need to prepare it
        NOTE: On wayland the window for some reason can automatically close when hidden on creation */
        if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)
            glfwWindowHint(GLFW_VISIBLE, !specs.ShowOnceRendererIsWorking);

        // if (m_WindowMode == WindowMode::Windowed)
        // {
        //     // If a valid position is specified, use it, otherwise set position to the middle of the primary monitor
        //     if (specs.WindowedPosition.has_value() && specs.WindowedPosition.value() != glm::ivec2(INT32_MAX))
        //     {
        //         m_Position = specs.WindowedPosition.value();
        //         DetectCurrentMonitor();
        //     }
        //     else
        //     {
        //         m_CurrentMonitor = Monitors::GetPrimary();
        //         auto vidMode = m_CurrentMonitor.GetCurrentVideoMode();
        //         m_Position = { vidMode.Size.Width / 2 - m_Size.Width / 2, vidMode.Size.Height / 2 - m_Size.Height / 2 };
        //     }

        //     // Initialize these to correct values
        //     m_LastWindowedPosition = m_Position;
        //     m_LastWindowedSize = m_Size;
        // }
        // else if (m_WindowMode == WindowMode::Borderless || m_WindowMode == WindowMode::Fullscreen)
        // {
        //     // If monitor index is specified, use it, otherwise use the primary monitor
        //     if (specs.FullscreenMonitorIndex.has_value())
        //         m_CurrentMonitor = Monitors::Get(specs.FullscreenMonitorIndex.value() - 1);
        //     else
        //         m_CurrentMonitor = Monitors::GetPrimary();

        //     // Set position to top left of specified monitor
        //     m_Position = m_CurrentMonitor.Position;

        //     // Force size to monitor size (this is necessary for Borderless to be fullscreen)
        //     m_Size = m_CurrentMonitor.GetCurrentVideoMode().Size;
        // }

        // Set glfwMonitor if we need to be fullscreen
        GLFWmonitor* glfwMonitor = nullptr;
        if ((glfwGetPlatform() == GLFW_PLATFORM_WAYLAND && (m_WindowMode == WindowMode::Fullscreen || m_WindowMode == WindowMode::Borderless)) || m_WindowMode == WindowMode::Fullscreen)
        {
            glfwMonitor = m_CurrentMonitor.GLFWMonitor;
        }

        m_GLFWWindow = glfwCreateWindow(static_cast<int>(specs.WindowedSize.Width), static_cast<int>(specs.WindowedSize.Height), specs.Title.c_str(), glfwMonitor, nullptr);
        
        PXL_ASSERT_MSG(m_GLFWWindow, "Failed to create GLFW window '{}'", specs.Title);
        PXL_LOG_INFO("Created GLFW window '{}' of size {}x{}", specs.Title, specs.WindowedSize.Width, specs.WindowedSize.Height);

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        InitCallbacks();

        // Set icon if supplied
        if (specs.IconPath.has_value())
        {
            auto image = FileSystem::LoadImageFile(specs.IconPath.value(), true);

            if (image)
                SetIcon(*image);
            else
                PXL_LOG_ERROR("Failed to load window icon file");
        }


#ifdef _WIN32
        Platform::Windows::EnableDarkModeIfSupported(m_GLFWWindow);
#endif

        // Init event and input systems
        auto& eventManager = Application::Get().GetEventManager();
        m_EventCallback = eventManager.GetEventSendCallback();
        m_InputSystem = std::make_shared<InputSystem>(m_GLFWWindow, eventManager.GetEventQueueCallback());
    }

    void Window::Close() const
    {
        glfwDestroyWindow(m_GLFWWindow);
        s_Windows.erase(std::find(s_Windows.begin(), s_Windows.end(), m_SelfHandle.lock()));
    }

    void Window::InitCallbacks()
    {
        glfwSetWindowCloseCallback(m_GLFWWindow, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_GLFWWindow, WindowResizeCallback);
        glfwSetWindowPosCallback(m_GLFWWindow, WindowPositionCallback);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, FramebufferResizeCallback);
        glfwSetWindowIconifyCallback(m_GLFWWindow, WindowIconifyCallback);
        glfwSetDropCallback(m_GLFWWindow, DropCallback);
        glfwSetCursorEnterCallback(m_GLFWWindow, CursorEnterCallback);
    }

    void Window::DetectCurrentMonitor()
    {
        PXL_PROFILE_SCOPE;

        Monitor bestMonitor = Monitors::GetPrimary();
        int32_t bestOverlap = 0;

        for (const auto& monitor : Monitors::GetAll())
        {
            auto vidmode = monitor.GetCurrentVideoMode();

            auto left = monitor.Position.x;
            auto right = monitor.Position.x + static_cast<int32_t>(vidmode.Size.Width);
            auto top = monitor.Position.y;
            auto bottom = monitor.Position.y + static_cast<int32_t>(vidmode.Size.Height);

            // Determine monitor on a best overlap basis. Sourced from https://stackoverflow.com/a/31526753
            auto xOverlap = std::max<int>(0, std::min<int>(m_Position.x + m_Size.Width, right) - std::max<int>(m_Position.x, left));
            auto yOverlap = std::max<int>(0, std::min<int>(m_Position.y + m_Size.Height, bottom) - std::max<int>(m_Position.y, top));

            auto overlap = xOverlap * yOverlap;

            if (bestOverlap < overlap)
            {
                bestOverlap = overlap;
                bestMonitor = monitor;
            }
        }

        if (bestOverlap == 0)
        {
            // If no correct monitor was found, then just leave it as it was
            PXL_LOG_WARN("Failed to determine window's current monitor");
            return;
        }

        m_CurrentMonitor = bestMonitor;
    }

    void Window::SetSizeLimits(const Size2D& minSize, const Size2D& maxSize)
    {
        glfwSetWindowSizeLimits(m_GLFWWindow, minSize.Width, minSize.Height, maxSize.Width, maxSize.Height);
    }

    void Window::SetPosition(int32_t xpos, int32_t ypos)
    {
        // NOTE: This function doesn't handle setting the window in out of bounds areas.
        glfwSetWindowPos(m_GLFWWindow, xpos, ypos);

        PXL_LOG_INFO("Manually set window position to {}, {}", xpos, ypos);
    }

    void Window::SetWindowMode(WindowMode mode)
    {
        if (mode == m_WindowMode)
            return;

        auto monitor = GetCurrentMonitor();
        auto nativeVidMode = monitor.GetCurrentVideoMode();

        switch (mode)
        {
            case WindowMode::Windowed:
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, m_LastWindowedPosition.x, m_LastWindowedPosition.y, m_LastWindowedSize.Width, m_LastWindowedSize.Height, GLFW_DONT_CARE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, true);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, true);
                break;

            case WindowMode::Borderless:
                if (glfwGetPlatform() == GLFW_PLATFORM_WIN32)
                {
                    // Use a borderless window on windows (this allows other applications to display on top of ours)
                    glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, false);
                    glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, false);
                    glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitor.Position.x, monitor.Position.y, nativeVidMode.Size.Width, nativeVidMode.Size.Height, GLFW_DONT_CARE);
                }
                else
                {
                    // Default to regular fullscreen otherwise (GLFW doesn't support setting window position on wayland anyway)
                    glfwSetWindowMonitor(m_GLFWWindow, monitor.GLFWMonitor, 0, 0, nativeVidMode.Size.Width, nativeVidMode.Size.Height, GLFW_DONT_CARE);
                }

                break;

            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, monitor.GLFWMonitor, 0, 0, nativeVidMode.Size.Width, nativeVidMode.Size.Height, nativeVidMode.RefreshRate);
                break;
        }

        m_WindowMode = mode;

        WindowModeChangeEvent event(m_SelfHandle.lock(), mode);
        m_EventCallback(event);

        PXL_LOG_INFO("Switched '{}' to {} window mode", GetTitle(), Utils::ToString(m_WindowMode));
    }

    void Window::SetMonitor(const Monitor& monitor)
    {
        // Get the current video mode of the specified monitor
        auto vidMode = monitor.GetCurrentVideoMode();

        int nextMonX, nextMonY;
        glfwGetMonitorWorkarea(monitor.GLFWMonitor, &nextMonX, &nextMonY, NULL, NULL);

        switch (m_WindowMode)
        {
            case WindowMode::Windowed:
                // Set window to the center of the specified monitor
                SetPosition(nextMonX + (vidMode.Size.Width / 2) - (m_Size.Width / 2), nextMonY + (vidMode.Size.Height / 2) - (m_Size.Height / 2));
                break;
            case WindowMode::Borderless:
                SetPosition(nextMonX, nextMonY);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, monitor.GLFWMonitor, 0, 0, vidMode.Size.Width, vidMode.Size.Height, vidMode.RefreshRate);
                break;
        }
    }

    void Window::NextWindowMode()
    {
        auto nextMode = static_cast<uint32_t>(m_WindowMode) + 1;
        auto lastMode = static_cast<uint32_t>(WindowMode::Fullscreen);
        SetWindowMode(static_cast<WindowMode>(nextMode % ++lastMode));
    }

    void Window::ToggleFullscreen()
    {
        if (m_WindowMode == WindowMode::Windowed || m_WindowMode == WindowMode::Borderless)
            SetWindowMode(WindowMode::Fullscreen);
        else if (m_WindowMode == WindowMode::Fullscreen)
            SetWindowMode(WindowMode::Windowed);
    }

    void Window::SetIcon(Image& image)
    {
        if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
            return;

        GLFWimage glfwImage;
        glfwImage.width = image.Metadata.Size.Width;
        glfwImage.height = image.Metadata.Size.Height;
        glfwImage.pixels = image.Buffer.data();

        glfwSetWindowIcon(m_GLFWWindow, 1, &glfwImage);
    }

    void Window::SetCursorMode(CursorMode mode)
    {
        glfwSetInputMode(m_GLFWWindow, GLFW_CURSOR, Utils::ToGLFWCursorMode(mode));
        PXL_LOG_INFO("Cursor mode set to {}", Utils::ToString(mode));
    }

    void Window::EnforceAspectRatio(uint32_t numerator, uint32_t denominator) const
    {
        glfwSetWindowAspectRatio(m_GLFWWindow, numerator, denominator);
    }

    Size2D Window::GetFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_GLFWWindow, &width, &height);

        return Size2D(width, height);
    }

    void Window::WindowCloseCallback(GLFWwindow* window)
    {
        PXL_PROFILE_SCOPE;

        auto windowHandle = static_cast<Window*>(glfwGetWindowUserPointer(window));

        PXL_LOG_INFO("Closing window '{}'", windowHandle->GetTitle());

        WindowCloseEvent event(windowHandle->m_SelfHandle.lock());
        windowHandle->m_EventCallback(event);

        windowHandle->Close();
    }

    void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        PXL_PROFILE_SCOPE;

        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (width == 0 && height == 0)
            return;

        windowInstance->m_Size.Width = windowInstance->m_WindowMode == WindowMode::Borderless ? width - 1 : width;
        windowInstance->m_Size.Height = height;

        if (windowInstance->m_WindowMode == WindowMode::Windowed)
            windowInstance->m_LastWindowedSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        WindowResizeEvent event(windowInstance->m_SelfHandle.lock(), { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
        windowInstance->m_EventCallback(event);
    }

    void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        PXL_PROFILE_SCOPE;

        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        WindowFBResizeEvent event(windowInstance->m_SelfHandle.lock(), { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
        windowInstance->m_EventCallback(event);
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        PXL_PROFILE_SCOPE;

        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        WindowMinimizeEvent event(windowInstance->m_SelfHandle.lock(), iconified);
        windowInstance->m_EventCallback(event);
    }

    void Window::WindowPositionCallback(GLFWwindow* window, int xpos, int ypos)
    {
        PXL_PROFILE_SCOPE;

        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        windowInstance->m_Position.x = xpos;
        windowInstance->m_Position.y = ypos;

        if (windowInstance->m_WindowMode == WindowMode::Windowed)
            windowInstance->m_LastWindowedPosition = { xpos, ypos };

        // NOTE: Retrieve an up-to-date iconification status, due to this callback being called before WindowIconifyCallback
        if (!glfwGetWindowAttrib(windowInstance->m_GLFWWindow, GLFW_ICONIFIED))
            windowInstance->DetectCurrentMonitor();

        WindowRepositionEvent event(windowInstance->m_SelfHandle.lock(), { xpos, ypos });
        windowInstance->m_EventCallback(event);
    }

    void Window::DropCallback(GLFWwindow* window, int count, const char** paths)
    {
        PXL_PROFILE_SCOPE;

        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        std::vector<std::string> stringPaths(count);
        for (int i = 0; i < count; i++)
            stringPaths[i] = paths[i];

        WindowPathDropEvent event(windowInstance->m_SelfHandle.lock(), stringPaths);
        windowInstance->m_EventCallback(event);
    }

    void Window::CursorEnterCallback(GLFWwindow* window, int entered)
    {
        PXL_PROFILE_SCOPE;

        auto windowHandle = static_cast<Window*>(glfwGetWindowUserPointer(window));

        WindowCursorEnterEvent event(windowHandle->m_SelfHandle.lock(), entered);
        windowHandle->m_EventCallback(event);
    }

    void Window::ResetPerFrameState()
    {
        PXL_PROFILE_SCOPE;

        for (auto& window : s_Windows)
            window->m_InputSystem->ResetCurrentState();
    }

    void Window::CloseAll()
    {
        auto windowCount = s_Windows.size();

        /* Window::Close() removes itself from the vector so we must
           always access the first element */
        for (size_t i = 0; i < windowCount; i++)
            s_Windows.front()->Close();
    }

    VkSurfaceKHR Window::CreateVKSurface(VkInstance instance)
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VK_CHECK(glfwCreateWindowSurface(instance, m_GLFWWindow, nullptr, &surface));

        PXL_LOG_INFO("Vulkan window surface created");

        return surface;
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        const auto maxWindowCount = 5;
        if (s_Windows.size() >= maxWindowCount)
        {
            PXL_LOG_ERROR("Failed to create window, the max window count has been reached");
            return nullptr;
        }

        auto window = std::make_shared<Window>(windowSpecs);

        window->m_SelfHandle = window;

        s_Windows.push_back(window);

        return window;
    }
}