#include "Config.h"

#include "../Renderer/RendererAPIType.h"

#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace pxl
{
    FrameworkSettings FrameworkConfig::m_FrameworkSettings = {};

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
        std::string rendererAPI = config["RendererAPI"].as<std::string>();
        if (rendererAPI == "OpenGL") // TODO: Custom data types with yaml-cpp
            m_FrameworkSettings.RendererAPI = RendererAPIType::OpenGL;
        else if (rendererAPI == "Vulkan")
            m_FrameworkSettings.RendererAPI = RendererAPIType::Vulkan;
        else
            m_FrameworkSettings.RendererAPI = RendererAPIType::None;

        // Window Mode
        std::string windowMode = config["WindowMode"].as<std::string>();
        if (windowMode == "Windowed") // TODO: Custom data types with yaml-cpp
            m_FrameworkSettings.WindowMode = WindowMode::Windowed;
        else if (windowMode == "Borderless")
            m_FrameworkSettings.WindowMode = WindowMode::Borderless;
        else if (windowMode == "Fullscreen")
            m_FrameworkSettings.WindowMode = WindowMode::Fullscreen;
        else
            m_FrameworkSettings.WindowMode = WindowMode::Windowed;

    }

    void FrameworkConfig::SaveToFile()
    {

    }
}