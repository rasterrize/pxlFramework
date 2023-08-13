#pragma once

#include "../Window.h"
#include <SDL.h>

namespace pxl
{
    class WindowSDL : public Window
    {
    public:
        virtual void Update() override;
        virtual void Close() override;

        virtual void SetSize(unsigned int width, unsigned int height) override;
        virtual void SetWindowMode(WindowMode winMode) override;
        virtual void SetMonitor(uint8_t monitorIndex) override;

        virtual void* GetNativeWindow() override { return m_Window; }
        virtual uint8_t GetAvailableMonitorCount() override;
    private:
        SDL_Window* m_Window;
    };
}