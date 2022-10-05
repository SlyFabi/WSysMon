#ifndef WSYSMON_AUTOSTARTAPI_H
#define WSYSMON_AUTOSTARTAPI_H

#include "../utils/Utils.h"

struct AutostartEntry {
    std::string path;
    std::string name;
    bool enabled;
};

class AutostartApi {
public:
    static std::vector<AutostartEntry> GetEntries();
    static std::optional<AutostartEntry> GetEntryAt(const std::string& path);

    static void SetEntryEnabled(const AutostartEntry& entry, bool enabled);

private:
    AutostartApi() = default;
};

#endif //WSYSMON_AUTOSTARTAPI_H
