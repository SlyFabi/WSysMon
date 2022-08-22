#ifndef WSYSMON_APPSETTINGS_H
#define WSYSMON_APPSETTINGS_H

#include "../utils/Utils.h"

struct Settings {
    bool displayProcList;
};

class AppSettings {
public:
    static Settings Get();
    static void Save(Settings settings);

private:
    static void Init();
    AppSettings() = default;

    static bool m_Initialized;
    static std::string m_ConfigPath;
};


#endif //WSYSMON_APPSETTINGS_H
