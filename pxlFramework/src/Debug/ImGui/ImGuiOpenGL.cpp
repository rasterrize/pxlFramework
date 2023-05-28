#include "ImGuiOpenGL.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../../Core/Window.h"

namespace pxl
{
    bool pxl_ImGui::s_Enabled;

    void pxl_ImGui::Init(GLFWwindow* window)
    {
        // Setup Dear ImGui context
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGuiIO();
        io = ImGui::GetIO(); (void)io;

        // Setup Platform/Renderer backends
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(window, true);

        s_Enabled = true;
    }

    void pxl_ImGui::Update()
    {
        if (!s_Enabled) 
            return;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
            
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void pxl_ImGui::Shutdown()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}