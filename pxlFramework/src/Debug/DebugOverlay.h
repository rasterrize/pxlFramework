#pragma once

#include "Core/Window.h"
#include "Renderer/Renderer.h"

namespace pxl
{
    class DebugOverlay
    {
    public:
        void Render(Window& window, Renderer& renderer);

        void ToggleVisibility() { m_Show = !m_Show; }

    private:
        bool m_Show = false;
    };
}