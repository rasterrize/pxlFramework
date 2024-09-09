#include "GUIOpenGL.h"

#include <backends/imgui_impl_opengl3.h>

namespace pxl
{
    GUIOpenGL::GUIOpenGL()
    {
        ImGui_ImplOpenGL3_Init("#version 460"); // should get glsl version from opengl
    }

    void GUIOpenGL::NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
    }

    void GUIOpenGL::Render()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUIOpenGL::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
    }
}