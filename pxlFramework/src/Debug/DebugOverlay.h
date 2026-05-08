#pragma once

#include "Core/Window.h"
#include "Renderer/Renderer.h"

namespace pxl
{
    class DebugOverlay
    {
    public:
        DebugOverlay(bool show = false)
            : m_Show(show)
        {
        }

        void Render(Window& window, Renderer& renderer);

        bool IsShown() const { return m_Show; }
        void ToggleVisibility() { m_Show = !m_Show; }

    private:
        bool m_Show = false;
    };
}