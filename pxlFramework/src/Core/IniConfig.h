#pragma once

#include <SimpleIni.h>

namespace pxl
{
    struct IniConfigSetting
    {
        std::string Section;
        std::string Key;
        std::string Value;
    };

    class IniConfig
    {
    public:
        IniConfig(const std::filesystem::path& path, const std::vector<IniConfigSetting>& defaultSettings);
        ~IniConfig();

        void SetAutoSave(bool value) { m_AutoSave = value; }

        std::string GetValue(const std::string& section, const std::string& key);
        void SetSetting(const IniConfigSetting& setting);

        void SetFilePath(const std::filesystem::path& path);

        bool Load();
        bool Save();

        void ResetToDefaults();
        bool ResetToDefault(const std::string& section, const std::string& key);

    private:
        CSimpleIni m_Ini = {};

        std::filesystem::path m_FilePath = {};

        std::vector<IniConfigSetting> m_DefaultSettings = {};

        bool m_AutoSave = true;
    };
}