#include "pxl_ImGui.h"

#include "ImGuiOpenGL.h"

#include "../../Core/Application.h"

#include <filesystem>

#include <backends/imgui_impl_glfw.h>

namespace pxl
{
    std::unique_ptr<ImGuiBase> pxl_ImGui::s_ImGuiRenderer = nullptr;
    std::shared_ptr<Window> pxl_ImGui::s_WindowHandle;
    RendererAPIType pxl_ImGui::s_RendererAPI = RendererAPIType::None;
    bool pxl_ImGui::s_Enabled = false;

    void pxl_ImGui::Init(std::shared_ptr<Window> window)
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
        {
            io.Fonts->AddFontFromFileTTF(fontFilename, 16);
        }
        else
        {
            io.Fonts->AddFontDefault();
        }

        ImGui::StyleColorsDark();

        GLFWwindow* windowHandle = static_cast<GLFWwindow*>(window->GetNativeWindow());

        // Create ImGui instance based on renderer API of the window
        switch (window->GetWindowSpecs().RendererAPI)
        {
            case RendererAPIType::None:
                Logger::LogError("Can't initialize ImGui because RendererAPI::None was specified");
                return;
            case RendererAPIType::OpenGL:
                ImGui_ImplGlfw_InitForOpenGL(windowHandle, true);
                s_ImGuiRenderer = std::make_unique<ImGuiOpenGL>();
                break;
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't initialize ImGui for Vulkan");
                return;
        }

        s_RendererAPI = window->GetWindowSpecs().RendererAPI;
        s_Enabled = true;

        Logger::LogInfo("ImGui initialized");
    }

    void pxl_ImGui::Update()
    {
        if (!s_Enabled) 
            return;

        // Start the Dear ImGui frame
        s_ImGuiRenderer->NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Application::Get().OnImGuiRender();
            
        // Rendering
        ImGui::Render();
        s_ImGuiRenderer->Render();
    }

    void pxl_ImGui::Shutdown()
    {
        // Cleanup
        if (!s_Enabled)
            return;

        s_ImGuiRenderer->Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        s_Enabled = false;

        Logger::LogInfo("ImGui shutdown");
    }
}