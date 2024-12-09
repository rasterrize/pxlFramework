#pragma once

#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPIType.h"
#include "Size.h"
#include "Window.h"

namespace pxl
{
    struct FrameworkSettings
    {
        // Window settings
        WindowMode WindowMode = WindowMode::Windowed;
        uint32_t MonitorIndex = 1;
        Size2D WindowSize = { 1280, 720 }; // TODO: set to Window::k_DefaultWindowedSize
        Size2D WindowPosition = { 0, 0 };
        float FullscreenRefreshRate = 60.0f;
        bool VSync = true;

        // Renderer settings
        RendererAPIType RendererAPI = RendererAPIType::OpenGL;
        FramerateCap FramerateCap = FramerateCap::Unlimited; // FPS cap will likely be implemented in window class
        uint32_t CustomFramerateCap = 60;
    };

    class FrameworkConfig
    {
    public:
        static void LoadDefaults() { s_Settings = {}; }

        static void LoadFromFile();
        static void SaveToFile();

        static FrameworkSettings& GetSettings() { return s_Settings; }
        static void SetSettings(const FrameworkSettings& settings) { s_Settings = settings; }

        static void SetAutoLoad(bool value) { s_AutoLoad = value; }
        static void SetAutoCreate(bool value) { s_AutoCreate = value; }
        static void SetAutoSave(bool value) { s_AutoSave = value; }

        // Do not include the 'FrameworkConfig.yaml' file name
        static void SetDirectory(const std::filesystem::path& path) { s_ConfigDirectory = path; }

    private:
        friend class Application;
        static void Init();
        static void Shutdown();

    private:
        static inline FrameworkSettings s_Settings = {};

        static inline std::filesystem::path s_ConfigDirectory;

        // Auto-load the config file on application start
        static inline bool s_AutoLoad = true;

        // Automatically create a config file if one doesn't exist
        static inline bool s_AutoCreate = true;

        // Auto-save when the app closes
        static inline bool s_AutoSave = true;
    };
}