#include "ImGuiOpenGL.h"

#include <backends/imgui_impl_opengl3.h>

namespace pxl
{
    ImGuiOpenGL::ImGuiOpenGL()
    {
        ImGui_ImplOpenGL3_Init("#version 460"); // should get glsl version from opengl
    }

    void ImGuiOpenGL::NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
    }

    void ImGuiOpenGL::Render()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }  

    void ImGuiOpenGL::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
    }
}