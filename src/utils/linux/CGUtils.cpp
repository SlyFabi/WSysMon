#include "CGUtils.h"

#include "../IOUtils.h"
#include <filesystem>
#include <unistd.h>

std::vector<int> CGUtils::GetAllPidsWithWindows() {
    auto cgPath = fmt::format("/sys/fs/cgroup/user.slice/user-{0}.slice/user@{0}.service/app.slice", getuid());
    if(!std::filesystem::exists(cgPath))
        return {};

    std::regex entryRegex(R"(app-(.+)-(.+)-(\d+))");
    std::smatch matches;

    auto result = std::vector<int>();
    for (const auto& dirEntry : std::filesystem::directory_iterator(cgPath)) {
        auto nameStr = dirEntry.path().stem().string();
        if(!std::regex_match(nameStr, matches, entryRegex))
            continue;

        for(const auto& pidStr : IOUtils::ReadAllLines(dirEntry.path().string() + "/cgroup.procs")) {
            auto pid = Utils::stringToInt(pidStr);
            if(pid > 0)
                result.emplace_back(pid);
        }
    }

    return result;
}
