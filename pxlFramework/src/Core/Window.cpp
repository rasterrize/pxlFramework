#include "Window.h"

#include "Application.h"
#include "Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanHelpers.h"
#include "Renderer/Vulkan/VulkanInstance.h"
#include "Utils/EnumStringHelper.h"
#include "Utils/FileSystem.h"

namespace pxl
{
    static constexpr uint8_t k_MaxWindowCount = 5;

    Window::Window(const WindowSpecs& specs)
        : m_Title(specs.Title), m_Size(specs.Size), m_WindowMode(specs.WindowMode), m_RendererAPI(specs.RendererAPI)
    {
        if (!s_Initialized)
            Window::Init();

        if (m_WindowMode == WindowMode::Windowed)
        {
            // If the position is specified, use it, otherwise set to the middle of primary monitor
            if (specs.Position.has_value())
            {
                m_Position = specs.Position.value();
                m_CurrentMonitor = GetPositionRelativeMonitor();
            }
            else
            {
                m_CurrentMonitor = GetPrimaryMonitor();
                auto vidMode = GetPrimaryMonitor().GetCurrentVideoMode();
                m_Position = { vidMode.Width / 2 - m_Size.Width / 2, vidMode.Height / 2 - m_Size.Height / 2 };
            }

            // Initialize these to correct values
            m_LastWindowedPosition = m_Position;
            m_LastWindowedSize = m_Size;
        }

        if (m_WindowMode == WindowMode::Borderless || m_WindowMode == WindowMode::Fullscreen)
        {
            // If monitor index is specified, use it, otherwise use the primary monitor
            m_CurrentMonitor = specs.MonitorIndex.has_value() ? s_Monitors[specs.MonitorIndex.value()] : GetPrimaryMonitor();

            // Set position to top left of specified monitor
            m_Position = GetCurrentMonitor().Position;

            // Force size to monitor size (this is necessary for Borderless to be fullscreen)
            m_Size = GetCurrentMonitor().GetCurrentVideoMode().GetSize();

            // Ensure LastWindowedPosition is the middle of the monitor
            auto vidMode = GetCurrentMonitor().GetCurrentVideoMode();
            m_LastWindowedPosition = { vidMode.Width / 2 - k_DefaultWindowedSize.Width / 2, vidMode.Height / 2 - k_DefaultWindowedSize.Height / 2 };
        }

        // Ensure we set glfwMonitor so the window gets created in exclusive fullscreen
        GLFWmonitor* glfwMonitor = m_WindowMode == WindowMode::Fullscreen ? GetCurrentMonitor().GLFWMonitor : nullptr;

        // Reset window hints so we don't get irregular behaviour
        glfwDefaultWindowHints();

        // Hide the window on creation as we will still need to prepare it
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Set window position on creation (not used in fullscreen)
        glfwWindowHint(GLFW_POSITION_X, m_Position.x);
        glfwWindowHint(GLFW_POSITION_Y, m_Position.y);

        // Set window hints based on renderer api
        switch (specs.RendererAPI)
        {
            case RendererAPIType::None:
                PXL_LOG_WARN(LogArea::Window, "RendererAPI type 'None' specified! Creating a GLFW window with no renderer api...");
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;

            case RendererAPIType::OpenGL:
#if PXL_DEBUG
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
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

        m_GLFWWindow = glfwCreateWindow(static_cast<int>(m_Size.Width), static_cast<int>(m_Size.Height), m_Title.c_str(), glfwMonitor, nullptr);

        PXL_ASSERT_MSG(m_GLFWWindow, "Failed to create GLFW window '{}'", m_Title);

        PXL_LOG_INFO(LogArea::Window, "Created GLFW window '{}' of size {}x{}", m_Title, m_Size.Width, m_Size.Height);

        // Set icon if supplied
        if (specs.IconPath.has_value())
        {
            auto image = FileSystem::LoadImageFile(specs.IconPath.value(), true);
            SetIcon(image);
        }

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        SetGLFWCallbacks();
        UpdateAspectRatio();
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
        {
            Application::Get().Close();
            return;
        }

        if (Renderer::IsInitialized() && Renderer::GetGraphicsContext() == m_GraphicsContext)
            Renderer::Shutdown();
    }

    void Window::SetGLFWCallbacks()
    {
        glfwSetWindowCloseCallback(m_GLFWWindow, WindowCloseCallback);
        glfwSetWindowSizeCallback(m_GLFWWindow, WindowResizeCallback);
        glfwSetWindowPosCallback(m_GLFWWindow, WindowPositionCallback);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, FramebufferResizeCallback);
        glfwSetWindowIconifyCallback(m_GLFWWindow, WindowIconifyCallback);
        glfwSetDropCallback(m_GLFWWindow, [](GLFWwindow* window, int count, const char** paths)
        {
            auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

            if (windowInstance->m_UserFileDropCallback)
            {
                std::vector<std::string> stringPaths(count);
                for (int i = 0; i < count; i++)
                    stringPaths[i] = paths[i];

                windowInstance->m_UserFileDropCallback(stringPaths);
            }
        });

        glfwSetKeyCallback(m_GLFWWindow, Input::GLFWKeyCallback);
        glfwSetMouseButtonCallback(m_GLFWWindow, Input::GLFWMouseButtonCallback);
        glfwSetScrollCallback(m_GLFWWindow, Input::GLFWScrollCallback);
        glfwSetCursorPosCallback(m_GLFWWindow, Input::GLFWCursorPosCallback);
    }

    void Window::SetStaticGLFWCallbacks()
    {
        glfwSetErrorCallback(GLFWErrorCallback);
        glfwSetMonitorCallback(MonitorCallback);
    }

    const Monitor& Window::GetPositionRelativeMonitor()
    {
        auto windowCenterX = m_Position.x + static_cast<int32_t>(m_Size.Width / 2);
        auto windowCenterY = m_Position.y + static_cast<int32_t>(m_Size.Height / 2);

        for (const auto& monitor : s_Monitors)
        {
            auto vidmode = monitor.GetCurrentVideoMode();

            auto left = monitor.Position.x;
            auto right = monitor.Position.x + static_cast<int32_t>(vidmode.Width);
            auto top = monitor.Position.y;
            auto bottom = monitor.Position.y + static_cast<int32_t>(vidmode.Height);

            if ((windowCenterX >= left && windowCenterX < right) && (windowCenterY >= top && windowCenterY < bottom))
                return monitor;
        }

        PXL_LOG_ERROR(LogArea::Window, "Failed to get window current monitor, returning primary monitor");
        return GetPrimaryMonitor();
    }

    void Window::SetSize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(m_GLFWWindow, width, height);

        // Check for successful window size change
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_GLFWWindow, &windowWidth, &windowHeight);

        PXL_ASSERT_MSG(windowWidth == static_cast<int>(width) && windowHeight == static_cast<int>(height), "Failed to change window '{}' resolution to {}x{}", m_Title, width, height);
    }

    void Window::SetSizeLimits(uint32_t minWidth, uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight)
    {
        glfwSetWindowSizeLimits(m_GLFWWindow, minWidth, minHeight, maxWidth, maxHeight);
    }

    void Window::SetPosition(int32_t xpos, int32_t ypos)
    {
        // NOTE: This function doesn't handle setting the window in out of bounds areas.
        glfwSetWindowPos(m_GLFWWindow, xpos, ypos);

        PXL_LOG_INFO(LogArea::Window, "Manually set window position to {}, {}", xpos, ypos);
    }

    void Window::SetWindowMode(WindowMode mode)
    {
        if (mode == m_WindowMode)
            return;

        auto currentMonitor = GetCurrentMonitor();
        auto vidMode = currentMonitor.GetCurrentVideoMode();

        // TODO: use monitor video mode / position
        int monitorX, monitorY, monitorWidth, monitorHeight;
        glfwGetMonitorWorkarea(currentMonitor.GLFWMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

        switch (mode)
        {
            case WindowMode::Windowed:
                m_WindowMode = WindowMode::Windowed;
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_TRUE);
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, m_LastWindowedPosition.x, m_LastWindowedPosition.y, m_LastWindowedSize.Width, m_LastWindowedSize.Height, GLFW_DONT_CARE);
                break;

            case WindowMode::Borderless:
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_FALSE);
                m_WindowMode = WindowMode::Borderless;

                /*  Using a 1px offset from the original window size tricks the operating system/drivers to think the window is regular and not fullscreen.
                    This obviously causes a 1px sliver on any right monitor, but it's worth it since no one will likely notice.
                    Another note: this will likely disable fullscreen features such as Adaptive Sync on the borderless window. */
                glfwSetWindowMonitor(m_GLFWWindow, nullptr, monitorX, monitorY, vidMode.Width + 1, vidMode.Height, GLFW_DONT_CARE);
                break;

            case WindowMode::Fullscreen:
                m_WindowMode = WindowMode::Fullscreen;
                glfwSetWindowMonitor(m_GLFWWindow, currentMonitor.GLFWMonitor, 0, 0, vidMode.Width, vidMode.Height, vidMode.RefreshRate);
                break;
        }

        PXL_LOG_INFO(LogArea::Window, "Switched '{}' to {} window mode", m_Title, EnumStringHelper::ToString(m_WindowMode));
    }

    void Window::SetMonitor(uint8_t monitorIndex)
    {
        SetMonitor(s_Monitors[monitorIndex - 1]);
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
                SetPosition(nextMonX + (vidMode.Width / 2) - (m_Size.Width / 2), nextMonY + (vidMode.Height / 2) - (m_Size.Height / 2));
                break;
            case WindowMode::Borderless:
                SetPosition(nextMonX, nextMonY);
                break;
            case WindowMode::Fullscreen:
                glfwSetWindowMonitor(m_GLFWWindow, monitor.GLFWMonitor, 0, 0, vidMode.Width, vidMode.Height, vidMode.RefreshRate);
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

    void Window::Minimize()
    {
        glfwIconifyWindow(m_GLFWWindow);
    }

    void Window::Maximize()
    {
        glfwMaximizeWindow(m_GLFWWindow);
    }

    void Window::Restore()
    {
        glfwRestoreWindow(m_GLFWWindow);
    }

    void Window::SetVisibility(bool value)
    {
        value ? glfwShowWindow(m_GLFWWindow) : glfwHideWindow(m_GLFWWindow);
    }

    void Window::SetIcon(const std::shared_ptr<Image>& image)
    {
        GLFWimage glfwImage;
        glfwImage.width = image->Metadata.Size.Width;
        glfwImage.height = image->Metadata.Size.Height;
        glfwImage.pixels = image->Buffer.data();

        glfwSetWindowIcon(m_GLFWWindow, 1, &glfwImage);
    }

    void Window::EnforceAspectRatio(uint32_t numerator, uint32_t denominator)
    {
        glfwSetWindowAspectRatio(m_GLFWWindow, numerator, denominator);
    }

    void Window::RequestUserAttention()
    {
        glfwRequestWindowAttention(m_GLFWWindow);
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

    std::vector<const char*> Window::GetVKRequiredInstanceExtensions()
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

        if (width == 0 && height == 0)
            return;

        windowInstance->m_Size.Width = windowInstance->m_WindowMode == WindowMode::Borderless ? width - 1 : width;
        windowInstance->m_Size.Height = height;
        windowInstance->UpdateAspectRatio();

        if (windowInstance->m_WindowMode == WindowMode::Windowed)
            windowInstance->m_LastWindowedSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        if (windowInstance->m_UserResizeCallback)
            windowInstance->m_UserResizeCallback({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    }

    void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (Renderer::IsInitialized())
        {
            // Use windowed borderless hack
            windowInstance->m_WindowMode == WindowMode::Borderless ? Renderer::ResizeViewport(0, 0, width - 1, height)
                                                                   : Renderer::ResizeViewport(0, 0, width, height);
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

            // Only recreate the swapchain if the fb size has actually changed
            if (static_cast<uint32_t>(width) != swapchainSpecs.Extent.width || static_cast<uint32_t>(height) != swapchainSpecs.Extent.height)
            {
                swapchain->SetExtent({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
                swapchain->FramebufferResized();
            }
        }
    }

    void Window::WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        windowInstance->m_Minimized = iconified;

        // Check if all windows are minimized
        bool allWindowsIconified = true;
        for (const auto& windowHandle : s_Windows)
        {
            if (windowHandle->m_Minimized == false)
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

    void Window::WindowPositionCallback(GLFWwindow* window, int xpos, int ypos)
    {
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        windowInstance->m_Position.x = xpos;
        windowInstance->m_Position.y = ypos;

        if (windowInstance->m_WindowMode == WindowMode::Windowed)
            windowInstance->m_LastWindowedPosition = { xpos, ypos };

        windowInstance->m_CurrentMonitor = windowInstance->GetPositionRelativeMonitor();
    }

    void Window::WindowDropCallback(GLFWwindow* window, int count, const char** paths)
    {
        auto windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (windowInstance->m_UserFileDropCallback)
        {
            std::vector<std::string> stringPaths(count);
            for (int i = 0; i < count; i++)
                stringPaths[i] = paths[i];

            windowInstance->m_UserFileDropCallback(stringPaths);
        }
    }

    void Window::MonitorCallback([[maybe_unused]] GLFWmonitor* monitor, [[maybe_unused]] int event)
    {
        UpdateMonitors();

        // TODO: Update all windows current index since they will likely be incorrect after monitor change.
    }

    void Window::Init()
    {
        if (!glfwInit())
        {
            PXL_LOG_ERROR(LogArea::Window, "Failed to initialize GLFW");
            return;
        }

        int major = 0, minor = 0, rev = 0;
        glfwGetVersion(&major, &minor, &rev);
        PXL_LOG_INFO(LogArea::Window, "GLFW initialized - Version {}.{}.{}", major, minor, rev);

        s_EventProcessFunc = glfwPollEvents;

        SetStaticGLFWCallbacks();

        UpdateMonitors();

        s_Initialized = true;
    }

    void Window::UpdateAll()
    {
        PXL_PROFILE_SCOPE;

        if (!s_Initialized || s_Windows.empty())
            return;

        for (const auto& window : s_Windows)
            window->Update();
    }

    void Window::UpdateMonitors()
    {
        int monitorCount;
        auto glfwMonitors = glfwGetMonitors(&monitorCount);

        for (uint8_t i = 0; i < monitorCount; i++)
        {
            Monitor monitor;
            monitor.GLFWMonitor = glfwMonitors[i];
            monitor.Index = i + 1;
            monitor.Name = glfwGetMonitorName(glfwMonitors[i]);
            glfwGetMonitorPos(glfwMonitors[i], &monitor.Position.x, &monitor.Position.y);

            int32_t widthMM, heightMM;
            glfwGetMonitorPhysicalSize(glfwMonitors[i], &widthMM, &heightMM);
            monitor.PhysicalSize = Size2D(static_cast<uint32_t>(widthMM), static_cast<uint32_t>(heightMM));

            int32_t vidModeCount = 0;
            auto vidModes = glfwGetVideoModes(glfwMonitors[i], &vidModeCount);

            for (int32_t v = 0; v < vidModeCount; v++)
                monitor.VideoModes.push_back(&vidModes[v]);

            monitor.IsPrimary = monitor.GLFWMonitor == glfwGetPrimaryMonitor();

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

        glfwTerminate();

        PXL_LOG_INFO(LogArea::Window, "GLFW terminated");
        PXL_LOG_INFO(LogArea::Window, "Window system shutdown");
    }

    std::shared_ptr<Window> Window::Create(const WindowSpecs& windowSpecs)
    {
        if (s_Windows.size() >= k_MaxWindowCount)
        {
            PXL_LOG_WARN(LogArea::Window, "Failed to create window, the max window count has been reached");
            return nullptr;
        }

        // clang-format off
        struct shared_window_enabler : public Window {
            shared_window_enabler(const WindowSpecs& specs) : Window(specs) {}
        };
        // clang-format on

        auto window = std::make_shared<shared_window_enabler>(windowSpecs);

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

            PXL_ASSERT_MSG(window->m_GraphicsContext, "Failed to create graphics context for window '{}'", windowSpecs.Title);
        }

        // Set the visibility now since we have a valid context
        window->SetVisibility(true);

        s_Windows.push_back(window);
        return window;
    }
}