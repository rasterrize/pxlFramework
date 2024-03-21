#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp> // decided to use glm here since its a good general math library as well
#include <vulkan/vulkan.h>

#include "WindowMode.h"
#include "../Renderer/GraphicsContext.h"
#include "../Renderer/RendererAPIType.h"

namespace pxl
{
    struct WindowSpecs
    {
        uint32_t Width = 640;
        uint32_t Height = 480;
        std::string Title = "";
        RendererAPIType RendererAPI = RendererAPIType::None;
        WindowMode WindowMode = WindowMode::Windowed;
        bool Minimized = false;
    };

    class Window
    {
    public:
        Window(const WindowSpecs& windowSpecs);

        void Close();

        void CreateGLFWWindow(const WindowSpecs& windowSpecs);

        void SetSize(uint32_t width, uint32_t height);

        void SetPosition(uint32_t x, uint32_t y);

        void SetWindowMode(WindowMode winMode);

        void SetMonitor(uint8_t monitorIndex);

        GLFWwindow* GetNativeWindow() const { return m_GLFWWindow; }

        std::shared_ptr<GraphicsContext> GetGraphicsContext() const { return m_GraphicsContext; }

        WindowSpecs GetWindowSpecs() const { return m_Specs; }

        uint32_t GetWidth() const { return m_Specs.Width; }

        uint32_t GetHeight() const { return m_Specs.Height; }

        WindowMode GetWindowMode() const { return m_Specs.WindowMode; }

        float GetAspectRatio() const { return ((float)m_Specs.Width / static_cast<float>(m_Specs.Height)); } // should this be cached in a variable? | could be updated every window resize callback
        glm::u32vec2 GetFramebufferSize();

        VkSurfaceKHR CreateVKWindowSurface(VkInstance instance); // Keeping vulkan here for now but obviously not ideal because it shouldnt be tied to window class
        std::vector<const char*> GetVKRequiredInstanceExtensions();

        void NextWindowMode();
        void ToggleFullscreen();

        void SetVSync(bool vsync) { m_GraphicsContext->SetVSync(vsync); }
        void ToggleVSync();

        void SetVisibility(bool value);

        void SetGLFWCallbacks();
        GLFWmonitor* GetCurrentMonitor();
    private:
        void Update();
    public:
        static int GetMonitorCount() { return s_MonitorCount; }

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);

        // GLFW callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
        static void MonitorCallback(GLFWmonitor* monitor, int event);
    private:
        friend class Application; // for UpdateAll()
        static void UpdateAll();

        static void Shutdown();
        static void PollEvents();
        static void WaitEvents();

        static void GetGLFWMonitors();
    private:
        GLFWwindow* m_GLFWWindow = nullptr;
        std::shared_ptr<GraphicsContext> m_GraphicsContext = nullptr;
        std::shared_ptr<Window> m_Handle = nullptr;

        WindowSpecs m_Specs = {};

        uint32_t m_LastWindowedWidth = 640; // TODO: make these change when the window size changes via user resize | also I think these might be unnecessary since GLFW stores the previous window size
        uint32_t m_LastWindowedHeight = 480;

        static uint8_t s_WindowCount;
        static uint8_t s_MonitorCount;

        static std::vector<std::shared_ptr<Window>> s_Windows;
        static GLFWmonitor** s_Monitors;

        // not sure
        static bool s_AllWindowsMinimized;
    };
}