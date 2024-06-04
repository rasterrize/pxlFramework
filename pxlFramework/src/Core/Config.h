#pragma once

#include "Window.h"
#include "../Renderer/RendererAPIType.h"

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

        static void LoadDefaults() { m_FrameworkSettings = {}; }

        static void LoadFromYAMLFile();
        static void SaveToFile();

        static const FrameworkSettings GetSettings() { return m_FrameworkSettings; } // return const ref?
        static void SetSettings(const FrameworkSettings& settings) { m_FrameworkSettings = settings; } // the settings could be dynamic, but that might too much effort for too little reward
    private:
        static FrameworkSettings m_FrameworkSettings;
    };
}