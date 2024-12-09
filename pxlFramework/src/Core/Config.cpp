#include "Config.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

#include "Utils/EnumStringHelper.h"

#ifdef PXL_DEBUG
    #define CONFIG_FILE_NAME_STRING "FrameworkConfig-dev.yaml"
#else
    #define CONFIG_FILE_NAME_STRING "FrameworkConfig.yaml"
#endif

namespace pxl
{
    void FrameworkConfig::Init()
    {
        if (std::filesystem::exists(CONFIG_FILE_NAME_STRING) && s_AutoLoad)
            LoadFromFile();
        else if (s_AutoCreate)
            SaveToFile();
    }

    void FrameworkConfig::Shutdown()
    {
        if (s_AutoSave)
            SaveToFile();
    }

    void FrameworkConfig::LoadFromFile()
    {
        YAML::Node config;

        config = YAML::LoadFile(CONFIG_FILE_NAME_STRING);

        // Renderer API
        auto rendererAPI = config["RendererAPI"].as<std::string>();
        if (rendererAPI == "OpenGL") // TODO: Custom data types with yaml-cpp
            s_Settings.RendererAPI = RendererAPIType::OpenGL;
        else if (rendererAPI == "Vulkan")
            s_Settings.RendererAPI = RendererAPIType::Vulkan;

        // Window Mode
        auto windowMode = config["WindowMode"].as<std::string>();
        if (windowMode == "Windowed") // TODO: Custom data types with yaml-cpp
            s_Settings.WindowMode = WindowMode::Windowed;
        else if (windowMode == "Borderless")
            s_Settings.WindowMode = WindowMode::Borderless;
        else if (windowMode == "Fullscreen")
            s_Settings.WindowMode = WindowMode::Fullscreen;

        s_Settings.VSync = config["VSync"].as<bool>();
    }

    void FrameworkConfig::SaveToFile()
    {
        YAML::Node saveNode;
        saveNode["RendererAPI"] = EnumStringHelper::ToString(s_Settings.RendererAPI);
        saveNode["WindowMode"] = EnumStringHelper::ToString(s_Settings.WindowMode);
        saveNode["VSync"] = s_Settings.VSync;

        std::ofstream configFile(s_ConfigDirectory.string() + CONFIG_FILE_NAME_STRING);

        configFile << saveNode;

        PXL_LOG_INFO(LogArea::Core, "Saved config file");
    }
}