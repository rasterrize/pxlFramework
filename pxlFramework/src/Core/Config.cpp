#include "Config.h"

#include <yaml-cpp/yaml.h>

namespace pxl
{
    FrameworkSettings FrameworkConfig::s_FrameworkSettings = {};

    void FrameworkConfig::Init()
    {
        // TODO: Check if FrameworkConfig.yaml exists, if not, create it (Then implement a way to turn this feature off)
        LoadFromYAMLFile();
    }

    void FrameworkConfig::LoadFromYAMLFile()
    {
        YAML::Node config;

        if (std::filesystem::exists("FrameworkConfig.yaml"))
        {
            config = YAML::LoadFile("FrameworkConfig.yaml");
        }
        else
        {
            PXL_LOG_ERROR(LogArea::Core, "Failed to find FrameworkConfig.yaml in working directory");
            return;
        }

        // Renderer API
        auto rendererAPI = config["RendererAPI"].as<std::string>();
        if (rendererAPI == "OpenGL") // TODO: Custom data types with yaml-cpp
            s_FrameworkSettings.RendererAPI = RendererAPIType::OpenGL;
        else if (rendererAPI == "Vulkan")
            s_FrameworkSettings.RendererAPI = RendererAPIType::Vulkan;

        // Window Mode
        auto windowMode = config["WindowMode"].as<std::string>();
        if (windowMode == "Windowed") // TODO: Custom data types with yaml-cpp
            s_FrameworkSettings.WindowMode = WindowMode::Windowed;
        else if (windowMode == "Borderless")
            s_FrameworkSettings.WindowMode = WindowMode::Borderless;
        else if (windowMode == "Fullscreen")
            s_FrameworkSettings.WindowMode = WindowMode::Fullscreen;
    }

    void FrameworkConfig::SaveToFile()
    {
    }
}