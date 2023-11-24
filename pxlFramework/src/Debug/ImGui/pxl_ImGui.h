#pragma once

#include "../../Core/Window.h"
#include "../../Renderer/RendererAPIType.h"

#include "ImGuiBase.h"

namespace pxl
{
    class pxl_ImGui
    {
    public:
        static void Init(std::shared_ptr<Window> window);
        static void Update();
        static void Shutdown();

        static std::shared_ptr<Window> GetWindowHandle() { return s_WindowHandle; }
    private:
        static std::unique_ptr<ImGuiBase> s_ImGuiRenderer;
        static std::shared_ptr<Window> s_WindowHandle;
        static RendererAPIType s_RendererAPI;
        static bool s_Enabled;
    };
}