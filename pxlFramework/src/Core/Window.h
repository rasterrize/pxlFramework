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
        unsigned int Width;
        unsigned int Height;
        std::string Title;
        RendererAPIType RendererAPI;
    };

    class Window
    {
    public:
        virtual void Update() = 0;
        virtual void Close() = 0;

        virtual void SetSize(unsigned int width, unsigned int height) = 0;
        virtual void SetWindowMode(WindowMode winMode) = 0;
        virtual void SetMonitor(uint8_t monitorIndex) = 0;

        virtual void* GetNativeWindow() = 0;
        virtual uint8_t GetAvailableMonitorCount() = 0;

        std::shared_ptr<GraphicsContext> GetGraphicsContext() { return m_GraphicsContext; }

        const unsigned int GetWidth() const { return m_WindowSpecs.Width; }
        const unsigned int GetHeight() const { return m_WindowSpecs.Height; }

        const WindowSpecs GetWindowSpecs() const { return m_WindowSpecs; }
        const WindowMode GetWindowMode() const { return m_WindowMode; }

        const float GetAspectRatio() const { return ((float)m_WindowSpecs.Width / m_WindowSpecs.Height); } // should be cached in a variable

        void NextWindowMode();
        void ToggleFullscreen();

        void SetVSync(bool vsync) { m_GraphicsContext->SetVSync(vsync); }
        void ToggleVSync();
    public:
        static void Shutdown(); // I don't think this should be public

        static std::shared_ptr<Window> Create(const WindowSpecs& windowSpecs);
    protected:
        Window(const WindowSpecs& windowSpecs);
        
        std::shared_ptr<GraphicsContext> m_GraphicsContext;

        WindowSpecs m_WindowSpecs;
        WindowMode m_WindowMode;
        bool m_Minimized = false;

        std::shared_ptr<Window> m_Handle;
    protected:
        static uint8_t s_WindowCount;
        static std::vector<std::shared_ptr<Window>> s_Windows;
    private:
        friend class Application;

        static void UpdateAll();
    };
}