#include "GUI.h"

#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

#include <filesystem>

#include "Core/Application.h"
#include "GUIOpenGL.h"
#include "Renderer/Renderer.h"

namespace pxl
{
    void GUI::Init(const std::shared_ptr<Window>& window)
    {
        PXL_ASSERT_MSG(window, "Can't initialize GUI with invalid window");
        PXL_ASSERT_MSG(Renderer::IsInitialized(), "Renderer must be initialized before GUI is initialized");

        s_WindowHandle = window;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename = NULL;

        // Set custom font
        const char* fontFilename = "resources/fonts/Roboto-Medium.ttf";
        std::filesystem::exists(fontFilename) ? io.Fonts->AddFontFromFileTTF(fontFilename, 16) : io.Fonts->AddFontDefault();

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

        Application::Get().OnGUIRender();
    }

    void GUI::Render()
    {
        if (!s_Enabled)
            return;

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