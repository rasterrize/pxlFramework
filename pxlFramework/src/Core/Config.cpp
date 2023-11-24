#include "Config.h"

#include "../Renderer/RendererAPIType.h"

#include <yaml-cpp/yaml.h>

namespace pxl
{
    FrameworkSettings FrameworkConfig::m_FrameworkSettings;

    void FrameworkConfig::Init()
    {
        // TODO: Check if FrameworkConfig.yaml exists, if not, create it (Then implement a way to turn this feature off)
        LoadFromYAMLFile();
    }

    void FrameworkConfig::LoadFromYAMLFile()
    {
        YAML::Node config = YAML::LoadFile("FrameworkConfig.yaml"); // crashes if the file isn't loaded, need to find a way around this

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