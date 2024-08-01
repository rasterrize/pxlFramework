#include "GUI.h"

#include <filesystem>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include "GUIOpenGL.h"
#include "Core/Application.h"

namespace pxl
{
    std::unique_ptr<GUIBase> GUI::s_ImGuiRenderer = nullptr;
    std::shared_ptr<Window> GUI::s_WindowHandle;
    RendererAPIType GUI::s_RendererAPI = RendererAPIType::None;
    bool GUI::s_Enabled = false;

    void GUI::Init(const std::shared_ptr<Window>& window)
    {
        s_WindowHandle = window;
        
        const char* fontFilename = "assets/fonts/Roboto-Medium.ttf";

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.IniFilename = NULL;
        if (std::filesystem::exists(fontFilename))
            io.Fonts->AddFontFromFileTTF(fontFilename, 16);
        else
            io.Fonts->AddFontDefault();

        ImGui::StyleColorsDark();

        auto glfwWindow = window->GetNativeWindow();

        // Create ImGui instance based on renderer API of the window
        switch (window->GetWindowSpecs().RendererAPI)
        {
            case RendererAPIType::None:
                PXL_LOG_ERROR(LogArea::Other, "Can't initialize ImGui because RendererAPI::None was specified");
                return;
            case RendererAPIType::OpenGL:
                ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
                s_ImGuiRenderer = std::make_unique<GUIOpenGL>();
                break;
            case RendererAPIType::Vulkan:
                PXL_LOG_ERROR(LogArea::Other, "Can't initialize ImGui for Vulkan");
                return;
        }

        s_RendererAPI = window->GetWindowSpecs().RendererAPI;
        s_Enabled = true;

        PXL_LOG_INFO(LogArea::Other, "Debug GUI using ImGui initialized");
    }

    void GUI::Update()
    {
        PXL_PROFILE_SCOPE;
        
        if (!s_Enabled) 
            return;

        // Start the Dear ImGui frame
        s_ImGuiRenderer->NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Application::Get().OnGuiRender();
    }

    void GUI::Render()
    {
        ImGui::Render();
        s_ImGuiRenderer->Render();
    }

    void GUI::Shutdown()
    {
        // Cleanup
        if (!s_Enabled)
            return;

        s_ImGuiRenderer->Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        s_Enabled = false;

        PXL_LOG_INFO(LogArea::Other, "GUI (ImGui) shutdown");
    }
}