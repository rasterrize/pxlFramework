#pragma once

#include "Window.h"
#include "Renderer/RendererAPIType.h"

namespace pxl
{
    struct FrameworkSettings
    {
        RendererAPIType RendererAPI = RendererAPIType::OpenGL;
        uint32_t MonitorIndex = 1;
        WindowMode WindowMode = WindowMode::Windowed;
    };

    class FrameworkConfig
    {
    public:
        static void Init();

        static void LoadDefaults() { s_FrameworkSettings = {}; }

        static void LoadFromYAMLFile();
        static void SaveToFile();

        // the settings could be dynamic, but that might too much effort for too little reward
        static const FrameworkSettings& GetSettings() { return s_FrameworkSettings; }  // return const ref?
        static void SetSettings(const FrameworkSettings& settings) { s_FrameworkSettings = settings; }
    private:
        static FrameworkSettings s_FrameworkSettings;
    };
}