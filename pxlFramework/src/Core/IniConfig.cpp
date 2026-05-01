#include "IniConfig.h"

namespace pxl
{
    IniConfig::IniConfig(const std::filesystem::path& path, const std::vector<IniConfigSetting>& defaultSettings)
        : m_FilePath(path), m_DefaultSettings(defaultSettings)
    {
        PXL_ASSERT_MSG(path.extension() == ".ini", "Configuration file extension was not .ini");

        ResetToDefaults();

        // Auto create the ini file if necessary
        if (!Load())
            Save();
    }

    IniConfig::~IniConfig()
    {
        if (m_AutoSave)
            Save();
    }

    std::string IniConfig::GetValue(const std::string& section, const std::string& key)
    {
        PXL_ASSERT(m_Ini.KeyExists(section.c_str(), key.c_str()));

        return m_Ini.GetValue(section.c_str(), key.c_str());
    }

    void IniConfig::SetSetting(const IniConfigSetting& setting)
    {
        m_Ini.SetValue(setting.Section.c_str(), setting.Key.c_str(), setting.Value.c_str());
    }

    void IniConfig::SetFilePath(const std::filesystem::path& path)
    {
        PXL_ASSERT_MSG(path.extension() == ".ini", "Configuration file extension was not .ini");

        m_FilePath = path;
    }

    bool IniConfig::Load()
    {
        if (m_Ini.LoadFile(m_FilePath.string().c_str()) != SI_OK)
        {
            PXL_LOG_ERROR("Failed to load ini config file");
            return false;
        }

        PXL_LOG_INFO("Loaded '{}' ini config", m_FilePath.string());

        return true;
    }

    bool IniConfig::Save()
    {
        if (m_Ini.SaveFile(m_FilePath.string().c_str()) != SI_OK)
        {
            PXL_LOG_ERROR("Failed to save ini config file");
            return false;
        }

        PXL_LOG_INFO("Saved '{}' ini config", m_FilePath.string());

        return true;
    }

    void IniConfig::ResetToDefaults()
    {
        for (const auto& setting : m_DefaultSettings)
            SetSetting(setting);
    }

    bool IniConfig::ResetToDefault(const std::string& section, const std::string& key)
    {
        for (const auto& defaultSetting : m_DefaultSettings)
        {
            if (defaultSetting.Section == section && defaultSetting.Key == key)
            {
                SetSetting(defaultSetting);
                return true;
            }
        }

        return false;
    }
}