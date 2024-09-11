#pragma once

#include "Core/Window.h"
#include "GUIBase.h"
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

        static bool IsInitialized() { return s_Enabled; }

        static std::shared_ptr<Window> GetWindowHandle() { return s_WindowHandle; }
    private:
        static inline bool s_Enabled = false;

        static inline std::unique_ptr<GUIBase> s_ImGuiRenderer = nullptr;

        static inline std::shared_ptr<Window> s_WindowHandle = nullptr;
    };
}