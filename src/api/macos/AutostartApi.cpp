#include "../AutostartApi.h"

std::vector<AutostartEntry> AutostartApi::GetEntries() {
    return {};
}

std::optional<AutostartEntry> AutostartApi::GetEntryAt(const std::string &path) {
    return {};
}

void AutostartApi::SetEntryEnabled(const AutostartEntry &entry, bool enabled) {
}
