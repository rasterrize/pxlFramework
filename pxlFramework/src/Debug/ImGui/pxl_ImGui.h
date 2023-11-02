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
    private:
        static std::unique_ptr<ImGuiBase> m_ImGuiRenderer;
        static RendererAPIType m_RendererAPI;
        static bool s_Enabled;
    };
}