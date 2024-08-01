#pragma once

#include "GUIBase.h"
#include "Core/Window.h"
#include "Renderer/RendererAPIType.h"

namespace pxl
{
    class GUI
    {
    public:
        static void Init(const std::shared_ptr<Window>& window);
        static void Update();
        static void Render();
        static void Shutdown();

        static std::shared_ptr<Window> GetWindowHandle() { return s_WindowHandle; }
    private:
        static std::unique_ptr<GUIBase> s_ImGuiRenderer;
        static std::shared_ptr<Window> s_WindowHandle;
        static RendererAPIType s_RendererAPI;
        static bool s_Enabled;
    };
}