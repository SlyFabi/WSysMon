#include "../AutostartApi.h"

#include <filesystem>
#include <unistd.h>
#include <pwd.h>

std::vector<AutostartEntry> AutostartApi::GetEntries() {
    auto homeDir = std::string(getpwuid(getuid())->pw_dir);
    auto asDir = homeDir + "/.config/autostart";

    auto result = std::vector<AutostartEntry>();
    if(std::filesystem::exists(asDir)) {
        for (const auto& dirEntry : std::filesystem::directory_iterator(asDir)) {
            auto entry = GetEntryAt(dirEntry.path());
            if(entry.has_value()) {
                result.emplace_back(entry.value());
            }
        }
    }
    return result;
}

std::optional<AutostartEntry> AutostartApi::GetEntryAt(const std::string &path) {
    auto entryPath = std::filesystem::path(path);
    if(!std::filesystem::exists(entryPath))
        return {};

    if(entryPath.extension().string() == ".desktop" || entryPath.extension().string() == ".desktop_disabled") {
        AutostartEntry entry{};
        entry.path = entryPath;
        entry.name = entryPath.stem();
        entry.enabled = entryPath.extension().string() == ".desktop";

        return entry;
    }

    return {};
}

void AutostartApi::SetEntryEnabled(const AutostartEntry &entry, bool enabled) {
    auto entryPath = std::filesystem::path(entry.path);
    if(!std::filesystem::exists(entryPath))
        return;

    auto newPath = std::filesystem::path(entry.path);
    if(enabled) {
        newPath.replace_extension(".desktop");
    } else {
        newPath.replace_extension(".desktop_disabled");
    }

    std::filesystem::rename(entryPath, newPath);
}
