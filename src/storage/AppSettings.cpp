#include "AppSettings.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <unistd.h>
#include <pwd.h>

bool AppSettings::m_Initialized = false;
std::string AppSettings::m_ConfigPath = {};
std::optional<Settings> AppSettings::m_SettingsCache = {};

Settings AppSettings::Get() {
    if(!m_Initialized)
        Init();
    if(m_SettingsCache.has_value())
        return m_SettingsCache.value();

    try {
        std::ifstream inFile(m_ConfigPath);
        nlohmann::json json;
        inFile >> json;

        auto settings = Settings();
        settings.displayProcList = json["displayProcList"];
        settings.useIECUnits = json["useIECUnits"];
        return settings;
    }
    catch (...) {
        spdlog::warn("Could not read app storage!");
    }

    auto def = Settings();
    def.displayProcList = false;
    def.useIECUnits = false;
    Save(def);
    return def;
}

void AppSettings::Save(Settings settings) {
    if(!m_Initialized)
        Init();
    try {
        std::ofstream outFile(m_ConfigPath);
        nlohmann::json json = {
                {"displayProcList", settings.displayProcList},
                {"useIECUnits", settings.useIECUnits},
        };
        outFile << std::setw(4) << json << std::endl;
        m_SettingsCache = settings;
    }
    catch (...) {
        spdlog::error("Could not write app storage!");
    }
}

void AppSettings::Init() {
    auto homeDir = std::string(getpwuid(getuid())->pw_dir);
    auto appDir = homeDir + "/.local/share/WSysMon";
    if(!std::filesystem::exists(appDir))
        std::filesystem::create_directories(appDir);

    m_ConfigPath = appDir + "/settings.json";
    m_Initialized = true;
}
