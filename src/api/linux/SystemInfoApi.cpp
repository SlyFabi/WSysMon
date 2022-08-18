#include "../SystemInfoApi.h"
#include <unistd.h>

#define KiB_TO_B 1024

std::map<int, CPUTimes> SystemInfoApi::m_CpuUsageDeltas;

long SystemInfoApi::GetTotalRam() {
    return ReadNamedEntry("/proc/meminfo", "MemTotal") * KiB_TO_B;
}

long SystemInfoApi::GetFreeRam() {
    return ReadNamedEntry("/proc/meminfo", "MemAvailable") * KiB_TO_B;
}

long SystemInfoApi::GetUsedRam() {
    return GetTotalRam() - GetFreeRam();
}

double SystemInfoApi::GetRamUsagePercent() {
    auto totalRam = (double)SystemInfoApi::GetTotalRam();
    auto usedRam = (double)SystemInfoApi::GetUsedRam();
    return usedRam / totalRam;
}

CPUTimes SystemInfoApi::GetCPUTimes(int cpuId) {
    auto data = IOUtils::ReadAllText("/proc/stat");
    auto split = Utils::stringSplit(data, "\n");
    if(split.empty())
        return {};

    CPUTimes times{};
    std::regex entryRegex(R"(cpu ?(\d+)? (\d+) (\d+) (\d+) (\d+) (\d+) (\d+) (\d+) (\d+))");
    std::smatch matches;
    for(const auto& entryStr : split) {
        if(std::regex_search(entryStr, matches, entryRegex)) {
            if(matches.size() < 10)
                continue;

            auto entryCpuIdStr = matches[1].str();
            int entryCpuId = -1;
            if(!entryCpuIdStr.empty())
                entryCpuId = std::stoi(entryCpuIdStr);

            auto user = std::stol(matches[2].str());
            auto nice = std::stol(matches[3].str());
            auto system = std::stol(matches[4].str());
            auto idle = std::stol(matches[5].str());
            auto iowait = std::stol(matches[6].str());
            auto irq = std::stol(matches[7].str());
            auto softirq = std::stol(matches[8].str());
            auto steal = std::stol(matches[9].str());

            if(entryCpuId == cpuId) {
                times.user = user;
                times.nice = nice;
                times.system = system;
                times.idle = idle;
                times.iowait = iowait;
                times.irq = irq;
                times.softirq = softirq;
                times.steal = steal;
                return times;
            }
        }
    }

    return times;
}

double SystemInfoApi::GetCPUUsagePercent() {
    return GetCPUUsagePercent(-1);
}

double SystemInfoApi::GetCPUUsagePercent(int cpuId) {
    auto cpuTimes = GetCPUTimes(cpuId);
    if(!Utils::mapContains(m_CpuUsageDeltas, cpuId)) {
        m_CpuUsageDeltas.insert(std::make_pair(cpuId, cpuTimes));
        return 0;
    }

    auto total = cpuTimes.user + cpuTimes.nice + cpuTimes.system + cpuTimes.idle + cpuTimes.iowait + cpuTimes.irq + cpuTimes.softirq + cpuTimes.steal;
    auto totalIdle = cpuTimes.idle + cpuTimes.iowait;

    auto prevStats = m_CpuUsageDeltas[cpuId];
    auto lastTotal = prevStats.user + prevStats.nice + prevStats.system + prevStats.idle + prevStats.iowait + prevStats.irq + prevStats.softirq + prevStats.steal;
    auto lastIdle = prevStats.idle + prevStats.iowait;

    auto deltaTotal = total - lastTotal;
    auto deltaIdle = totalIdle - lastIdle;

    m_CpuUsageDeltas[cpuId] = cpuTimes;

    auto cpuUsed = deltaTotal - deltaIdle;
    return 100. * (double)cpuUsed / (double)deltaTotal;
}

long SystemInfoApi::GetCPUClock() {
    long maxClock = 0;
    for(int i = 0; i < GetNumCPUs(); i++) {
        auto clock = GetCPUClock(i);
        if(clock > maxClock)
            maxClock = clock;
    }

    return maxClock;
}

long SystemInfoApi::GetCPUClock(int cpuId) {
    auto mHzStr = ReadCPUEntry(cpuId, "cpu MHz");
    auto mHz = std::stod(mHzStr);
    return (long)mHz * 1000000;
}

long SystemInfoApi::GetMaxCPUClock() {
    auto maxFreqStr = IOUtils::ReadAllText("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
    return Utils::stringToLong(maxFreqStr) * 1000;
}

std::string SystemInfoApi::GetCPUName() {
    return ReadCPUEntry(0, "model name");
}

int SystemInfoApi::GetNumCPUs() {
    return std::stoi(ReadCPUEntry(0, "cpu cores"));
}

int SystemInfoApi::GetNumCPUsLogical() {
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}

int SystemInfoApi::GetSocketCount() {
    int maxSocketId = 0;
    for(int i = 0; i < GetNumCPUs(); i++) {
        auto socketStr = ReadCPUEntry(i, "physical id");
        auto socketId = std::stoi(socketStr);
        if(socketId > maxSocketId)
            maxSocketId = socketId;
    }

    return maxSocketId + 1;
}

long SystemInfoApi::GetUptimeMS() {
    auto data = IOUtils::ReadAllText("/proc/uptime");
    auto split = Utils::stringSplit(data, " ");

    auto uptimeSeconds = std::stod(split[0]);
    return (long)(uptimeSeconds * 1000.);
}

long SystemInfoApi::ReadNamedEntry(const std::string& path, const std::string& name) {
    auto data = IOUtils::ReadAllText(path);
    auto split = Utils::stringSplit(data, "\n");

    std::regex entryRegex("(.+): *(\\d+)");
    std::smatch matches;
    for(const auto& entryStr : split) {
        if(std::regex_search(entryStr, matches, entryRegex)) {
            auto entryName = matches[1].str();
            auto entryVal = matches[2].str();

            if(name == entryName) {
                return std::stol(entryVal);
            }
        }
    }

    return 0;
}

std::string SystemInfoApi::ReadCPUEntry(int cpuId, const std::string& name) {
    auto data = IOUtils::ReadAllText("/proc/cpuinfo");
    auto cpuSplit = Utils::stringSplit(data, "\n\n");

    std::regex entryRegex("([A-Za-z _]+)\\t+: (.+)");
    std::smatch matches;
    for(const auto& cpuDataStr : cpuSplit) {
        auto split = Utils::stringSplit(cpuDataStr, "\n");

        auto entryCpuId = -1;
        for(const auto& entryStr : split) {
            if(std::regex_search(entryStr, matches, entryRegex)) {
                auto entryName = matches[1].str();
                auto entryVal = matches[2].str();

                if(entryName == "processor") {
                    entryCpuId = std::stoi(entryVal);
                }

                if(entryCpuId == cpuId && name == entryName) {
                    return entryVal;
                }
            }
        }
    }

    return "";
}
