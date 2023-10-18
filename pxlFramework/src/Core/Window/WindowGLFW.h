#pragma once

#include "../Window.h"

#include <GLFW/glfw3.h>

namespace pxl
{
    class WindowGLFW : public Window
    {
    public:
        WindowGLFW(const WindowSpecs& windowSpecs);

        virtual void Close() override;

        virtual void SetSize(uint32_t width, uint32_t height) override;
        virtual void SetPosition(uint32_t x, uint32_t y) override;
        virtual void SetWindowMode(WindowMode winMode) override;
        virtual void SetMonitor(uint8_t monitorIndex) override;
        
        virtual void* GetNativeWindow() override { return m_Window; }

    private:
        friend class Window;
        
        virtual void Update() override;
        static void ProcessEvents();

        void CreateGLFWWindow(const WindowSpecs& windowSpecs);
        void SetGLFWCallbacks();
        GLFWmonitor* GetCurrentMonitor();

        // Window callbacks
        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);

        // Monitor callbacks
        static void MonitorCallback(GLFWmonitor* monitor, int event);

    private:
        GLFWwindow* m_Window;
        uint8_t m_MonitorIndex = 1;

        static uint8_t s_GLFWWindowCount;
        static GLFWmonitor** s_Monitors;
    };
}