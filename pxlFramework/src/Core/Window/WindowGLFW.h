#pragma once

#include <GLFW/glfw3.h>

#include "../Window.h"

namespace pxl
{
    class WindowGLFW : public Window
    {
    public:
        WindowGLFW(const WindowSpecs& windowSpecs);

        virtual void Close() override;

        virtual uint8_t GetAvailableMonitorCount() override { return (uint8_t)s_MonitorCount; }

        virtual void SetSize(unsigned int width, unsigned int height) override;
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
        uint8_t m_MonitorIndex;

        static uint8_t s_GLFWWindowCount;
        static GLFWmonitor** s_Monitors;
        static int s_MonitorCount;
    };
}