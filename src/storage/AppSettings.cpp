#include "AppSettings.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <unistd.h>
#include <pwd.h>

bool AppSettings::m_Initialized = false;
std::string AppSettings::m_ConfigPath = {};

Settings AppSettings::Get() {
    if(!m_Initialized)
        Init();
    try {
        std::ifstream inFile(m_ConfigPath);
        nlohmann::json json;
        inFile >> json;

        auto settings = Settings();
        settings.displayProcList = json["displayProcList"];
        return settings;
    }
    catch (...) {
        spdlog::warn("Could not read app storage!");
    }

    auto def = Settings();
    def.displayProcList = false;
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
        };
        outFile << std::setw(4) << json << std::endl;
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
