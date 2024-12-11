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
        if (config["RendererAPI"].IsDefined())
        {
            auto rendererAPI = config["RendererAPI"].as<std::string>();
            if (rendererAPI == "OpenGL")
                s_Settings.RendererAPI = RendererAPIType::OpenGL;
            else if (rendererAPI == "Vulkan")
                s_Settings.RendererAPI = RendererAPIType::Vulkan;
        }

        // Window Mode
        if (config["WindowMode"].IsDefined())
        {
            auto windowMode = config["WindowMode"].as<std::string>();
            if (windowMode == "Windowed")
                s_Settings.WindowMode = WindowMode::Windowed;
            else if (windowMode == "Borderless")
                s_Settings.WindowMode = WindowMode::Borderless;
            else if (windowMode == "Fullscreen")
                s_Settings.WindowMode = WindowMode::Fullscreen;
        }

        // VSync
        if (config["VSync"].IsDefined())
            s_Settings.VSync = config["VSync"].as<bool>();

        // Window Size
        if (config["WindowWidth"].IsDefined())
        {
            auto width = config["WindowWidth"].as<uint32_t>();
            auto height = config["WindowHeight"].as<uint32_t>();
            s_Settings.WindowSize = { width, height };
        }

        // Window Position
        if (config["WindowPositionX"].IsDefined() && config["WindowPositionY"].IsDefined())
        {
            auto x = config["WindowPositionX"].as<int32_t>();
            auto y = config["WindowPositionY"].as<int32_t>();
            s_Settings.WindowPosition = { x, y };
        }

        // Fullscreen Refresh Rate
        if (config["FullscreenRefreshRate"].IsDefined())
            s_Settings.FullscreenRefreshRate = config["FullscreenRefreshRate"].as<uint32_t>();

        // Fullscreen Monitor
        if (config["FullscreenMonitor"].IsDefined())
            s_Settings.MonitorIndex = config["FullscreenMonitor"].as<uint32_t>();

        // Custom FPS Cap
        if (config["CustomFPSCap"].IsDefined())
            s_Settings.CustomFramerateCap = config["CustomFPSCap"].as<uint32_t>();
    }

    void FrameworkConfig::SaveToFile()
    {
        YAML::Node saveNode;
        saveNode["RendererAPI"] = EnumStringHelper::ToString(s_Settings.RendererAPI);
        saveNode["WindowMode"] = EnumStringHelper::ToString(s_Settings.WindowMode);
        saveNode["VSync"] = s_Settings.VSync;
        saveNode["WindowPositionX"] = s_Settings.WindowPosition.x;
        saveNode["WindowPositionY"] = s_Settings.WindowPosition.y;
        saveNode["WindowWidth"] = s_Settings.WindowSize.Width;
        saveNode["WindowHeight"] = s_Settings.WindowSize.Height;
        saveNode["FullscreenRefreshRate"] = s_Settings.FullscreenRefreshRate;
        saveNode["FullscreenMonitor"] = s_Settings.MonitorIndex;
        saveNode["CustomFPSCap"] = s_Settings.CustomFramerateCap;

        if (std::filesystem::exists(CONFIG_FILE_NAME_STRING))
            std::filesystem::remove(CONFIG_FILE_NAME_STRING);

        std::ofstream configFile(s_ConfigDirectory.string() + CONFIG_FILE_NAME_STRING);

        configFile << saveNode;

        PXL_LOG_INFO(LogArea::Core, "Saved config file");
    }
}