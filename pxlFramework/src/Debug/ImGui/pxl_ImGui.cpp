#include "pxl_ImGui.h"

#include "ImGuiOpenGL.h"

#include "../../Core/Application.h"

#include <filesystem>

#include <backends/imgui_impl_glfw.h>

namespace pxl
{
    std::unique_ptr<ImGuiBase> pxl_ImGui::m_ImGuiRenderer = nullptr;
    RendererAPIType pxl_ImGui::m_RendererAPI = RendererAPIType::None;
    bool pxl_ImGui::s_Enabled = false;

    void pxl_ImGui::Init(std::shared_ptr<Window> window)
    {
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
                m_ImGuiRenderer = std::make_unique<ImGuiOpenGL>();
                break;
            case RendererAPIType::Vulkan:
                Logger::LogError("Can't initialize ImGui for Vulkan");
                return;
        }

        m_RendererAPI = window->GetWindowSpecs().RendererAPI;
        s_Enabled = true;

        Logger::LogInfo("ImGui Initialized");
    }

    void pxl_ImGui::Update()
    {
        if (!s_Enabled) 
            return;

        // Start the Dear ImGui frame
        m_ImGuiRenderer->NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Application::Get().OnImGuiRender();
            
        // Rendering
        ImGui::Render();
        m_ImGuiRenderer->Render();
    }

    void pxl_ImGui::Shutdown()
    {
        // Cleanup
        if (!s_Enabled)
            return;

        m_ImGuiRenderer->Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        s_Enabled = false;
    }
}