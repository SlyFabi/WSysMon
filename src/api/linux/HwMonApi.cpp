#include "../HwMonApi.h"
#include "../../utils/IOUtils.h"
#include <filesystem>

std::map<int, std::string> g_HwMonMap;

int HwMonApi::GetNumDevices() {
    if(!std::filesystem::exists("/sys/class/hwmon"))
        return 0;

    int count = 0;
    for (const auto& dirEntry : std::filesystem::directory_iterator("/sys/class/hwmon")) {
        g_HwMonMap[count] = dirEntry.path().string();
        count++;
    }
    return count;
}

std::string HwMonApi::GetDeviceName(int id) {
    auto path = g_HwMonMap[id];
    return Utils::stringTrim(IOUtils::ReadAllText(path + "/name"));
}

std::string HwMonApi::FindInputPath(int id, const std::string &inputName) {
    auto path = g_HwMonMap[id];
    for (const auto& dirEntry : std::filesystem::directory_iterator(path)) {
        if(!Utils::stringEndsWith(dirEntry.path().string(), "_label"))
            continue;

        auto entryName = IOUtils::ReadAllText(dirEntry.path());
        entryName = Utils::stringTrim(entryName);
        if(entryName == inputName) {
            auto inputPath = Utils::stringReplace(dirEntry.path(), "_label", "_input");
            if(std::filesystem::exists(inputPath))
                return inputPath;
        }
    }

    return {};
}
