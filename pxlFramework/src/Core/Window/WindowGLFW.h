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
        virtual void SetSize(unsigned int width, unsigned int height) override;

        virtual void SetWindowMode(WindowMode winMode) override;
        virtual void SetMonitor(unsigned int monitorIndex) override;
        
        virtual void* GetNativeWindow() override { return m_Window; }

        virtual std::shared_ptr<GraphicsContext> GetGraphicsContext() override { return m_GraphicsContext; }
        
    private:
        friend class Window;
        
        virtual void Update() override;
        static void ProcessEvents();

        void CreateGLFWWindow(const WindowSpecs& windowSpecs);
        void SetGLFWCallbacks();

        static void WindowCloseCallback(GLFWwindow* window);
        static void WindowResizeCallback(GLFWwindow* window, int width, int height);
        static void WindowIconifyCallback(GLFWwindow* window, int iconification);
    private:
        GLFWwindow* m_Window;

        static uint8_t s_GLFWWindowCount;
        static GLFWmonitor** s_Monitors;
        static int s_MonitorCount;
    };
}