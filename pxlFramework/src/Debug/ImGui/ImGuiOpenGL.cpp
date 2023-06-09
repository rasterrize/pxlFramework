#include "ImGuiOpenGL.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../../Core/Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace pxl
{
    bool pxl_ImGui::s_Enabled = false;
    GLFWwindow* pxl_ImGui::m_WindowHandle;

    void pxl_ImGui::Init(std::shared_ptr<Window> window)
    {
        m_WindowHandle = window->GetNativeWindow();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.IniFilename = NULL;

        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        Logger::LogInfo("ImGui Initialized");

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

        Application::Get().OnImGuiRender();
        //ImGui::ShowDemoWindow();
            
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void pxl_ImGui::Shutdown()
    {
        // Cleanup
        if (!s_Enabled)
            return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        s_Enabled = false;
    }
}