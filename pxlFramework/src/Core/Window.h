#pragma once

#include "../Renderer/GraphicsContext.h"
#include "../Renderer/RendererAPIType.h"

namespace pxl
{
    enum class WindowMode
    {
        Windowed,
        Borderless,
        Fullscreen
    };

    struct WindowSpecs
    {
        uint32_t Width;
        uint32_t Height;
        std::string Title;
        RendererAPIType RendererAPI;
    };

    class Window
    {
    public:
        virtual void Update() = 0;
        virtual void Close() = 0;

        virtual void SetSize(uint32_t width, uint32_t height) = 0;
        virtual void SetPosition(uint32_t x, uint32_t y) = 0;
        virtual void SetWindowMode(WindowMode winMode) = 0;
        virtual void SetMonitor(uint8_t monitorIndex) = 0;

        virtual void* GetNativeWindow() = 0;

        std::shared_ptr<GraphicsContext> GetGraphicsContext() { return m_GraphicsContext; }

        const uint32_t GetWidth() const { return m_WindowSpecs.Width; }
        const uint32_t GetHeight() const { return m_WindowSpecs.Height; }

        const WindowSpecs GetWindowSpecs() const { return m_WindowSpecs; }
        const WindowMode GetWindowMode() const { return m_WindowMode; }

        const float GetAspectRatio() const { return ((float)m_WindowSpecs.Width / m_WindowSpecs.Height); } // should be cached in a variable

        void NextWindowMode();
        void ToggleFullscreen();

        void SetVSync(bool vsync) { m_GraphicsContext->SetVSync(vsync); }
        void ToggleVSync();
    public:
        static void Shutdown(); // I don't think this should be public

        static int GetMonitorCount() { return s_MonitorCount; }

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);
    protected:
        Window(const WindowSpecs& windowSpecs);
        
        std::shared_ptr<GraphicsContext> m_GraphicsContext;

        WindowSpecs m_WindowSpecs;
        WindowMode m_WindowMode;
        bool m_Minimized = false;

        uint32_t m_LastWindowedWidth = m_WindowSpecs.Width; // TODO: make these change when the window size changes via user resize
        uint32_t m_LastWindowedHeight = m_WindowSpecs.Height;

        std::shared_ptr<Window> m_Handle;
    protected:
        static uint8_t s_WindowCount;
        static std::vector<std::shared_ptr<Window>> s_Windows;
        
        static int s_MonitorCount;
    private:
        friend class Application;

        static void UpdateAll();
    };
}