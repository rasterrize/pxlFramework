#pragma once

#include <SimpleIni.h>

#include <magic_enum/magic_enum.hpp>

namespace pxl
{
    struct IniConfigDefault
    {
        std::string Section;
        std::string Value;
    };

    template<typename SettingEnum>
    class IniConfig
    {
    public:
        IniConfig(const std::filesystem::path& path)
        {
            SetFilePath(path);

            if (!std::filesystem::exists(path))
                Save();
        }

        ~IniConfig()
        {
            if (m_AutoSave)
                Save();
        }

        bool GetBoolValue(SettingEnum setting)
        {
            if (!KeyExists(setting))
                return false;

            return m_Ini.GetBoolValue(GetSection(setting), magic_enum::enum_name(setting).data());
        }

        uint32_t GetUInt32Value(SettingEnum setting)
        {
            if (!KeyExists(setting))
                return UINT32_MAX;

            return m_Ini.GetLongValue(GetSection(setting), magic_enum::enum_name(setting).data());
        }

        int32_t GetInt32Value(SettingEnum setting)
        {
            if (!KeyExists(setting))
                return INT32_MAX;

            return m_Ini.GetLongValue(GetSection(setting), magic_enum::enum_name(setting).data());
        }

        template<typename T>
        T GetEnumValue(SettingEnum setting)
        {
            auto settingName = magic_enum::enum_name(setting);
            auto valueString = m_Ini.GetValue(GetSection(setting), settingName.data());
            auto value = magic_enum::enum_cast<T>(valueString);
            if (value.has_value())
            {
                return value.value();
            }
            else
            {
                PXL_LOG_ERROR("Failed to parse {} in INI config", settingName);
                return T();
            }
        }

        template<typename T>
        void SetValueEnum(SettingEnum setting, T value)
        {
            auto settingName = magic_enum::enum_name(setting);
            auto valueName = magic_enum::enum_name(value);
            m_Ini.SetValue(GetSection(setting), settingName.data(), valueName.data());
        }

        void SetValue(SettingEnum setting, bool value)
        {
            auto settingName = magic_enum::enum_name(setting);
            m_Ini.SetBoolValue(GetSection(setting), settingName.data(), value);
        }

        void SetValue(SettingEnum setting, float value)
        {
            SetValue(setting, static_cast<double>(value));
        }

        void SetValue(SettingEnum setting, double value)
        {
            auto settingName = magic_enum::enum_name(setting);
            m_Ini.SetDoubleValue(GetSection(setting), setting.data(), value);
        }

        void SetValue(SettingEnum setting, int32_t value)
        {
            SetValue(setting, static_cast<long>(value));
        }

        void SetValue(SettingEnum setting, uint32_t value)
        {
            SetValue(setting, static_cast<long>(value));
        }

        void SetValue(SettingEnum setting, long value)
        {
            auto settingName = magic_enum::enum_name(setting);
            m_Ini.SetLongValue(GetSection(setting), settingName.data(), value);
        }

        template<typename T>
        void SetDefaultEnum(std::string_view section, SettingEnum setting, T value)
        {
            m_Defaults[setting] = { section.data(), magic_enum::enum_name(value).data() };
            SetValueEnum<T>(setting, value);
        }

        void SetDefault(std::string_view section, SettingEnum setting, bool value)
        {
            m_Defaults[setting] = { section.data(), std::format("{}", value) };
            SetValue(setting, value);
        }

        void SetDefault(std::string_view section, SettingEnum setting, float value)
        {
            m_Defaults[setting] = { section.data(), std::format("{}", value) };
            SetValue(setting, value);
        }

        void SetDefault(std::string_view section, SettingEnum setting, double value)
        {
            m_Defaults[setting] = { section.data(), std::format("{}", value) };
            SetValue(setting, value);
        }

        void SetDefault(std::string_view section, SettingEnum setting, int32_t value)
        {
            m_Defaults[setting] = { section.data(), std::format("{}", value) };
            SetValue(setting, value);
        }

        void SetDefault(std::string_view section, SettingEnum setting, uint32_t value)
        {
            m_Defaults[setting] = { section.data(), std::format("{}", value) };
            SetValue(setting, value);
        }

        void SetFilePath(const std::filesystem::path& path)
        {
            PXL_ASSERT_MSG(path.extension() == ".ini", "Configuration file extension was not .ini");

            if (!path.parent_path().empty())
                std::filesystem::create_directory(path.parent_path());

            m_FilePath = path;
        }

        void SetAutoSave(bool value) { m_AutoSave = value; }

        bool Load()
        {
            if (m_Ini.LoadFile(m_FilePath.string().c_str()) != SI_OK)
            {
                PXL_LOG_ERROR("Failed to load ini config file");
                return false;
            }

            PXL_LOG_INFO("Loaded '{}' ini config", m_FilePath.string());

            return true;
        }

        bool Save()
        {
            if (m_Ini.SaveFile(m_FilePath.string().c_str()) != SI_OK)
            {
                PXL_LOG_ERROR("Failed to save ini config file");
                return false;
            }

            PXL_LOG_INFO("Saved '{}' ini config", m_FilePath.string());

            return true;
        }

    private:
        bool KeyExists(SettingEnum setting)
        {
            return m_Ini.KeyExists(GetSection(setting), magic_enum::enum_name(setting).data());
        }

        const char* GetSection(SettingEnum setting)
        {
            return m_Defaults.at(setting).Section.c_str();
        }

    private:
        CSimpleIni m_Ini = {};

        std::filesystem::path m_FilePath = {};

        std::unordered_map<SettingEnum, IniConfigDefault> m_Defaults;

        bool m_AutoSave = true;
    };
}