#ifndef WSYSMON_SYSTEMINFOAPI_H
#define WSYSMON_SYSTEMINFOAPI_H

#include <regex>
#include <string>
#include "../utils/IOUtils.h"

struct CPUTimes {
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;
    long steal;
    long guest;
};

class SystemInfoApi {
public:
    static long GetTotalRam();
    static long GetFreeRam();
    static long GetUsedRam();

    static double GetRamUsagePercent();

    static CPUTimes GetCPUTimes(int cpuId);
    static double GetCPUUsagePercent();
    static double GetCPUUsagePercent(int cpuId);

    static long GetCPUClock();
    static long GetCPUClock(int cpuId);
    static long GetMaxCPUClock();
    static double GetCPUTemperature();

    static std::string GetCPUName();
    static int GetNumCPUs();
    static int GetNumCPUsLogical();
    static int GetSocketCount();

    static long GetUptimeMS();

private:
    static long ReadNamedEntry(const std::string& path, const std::string& name);
    static std::string ReadCPUEntry(int cpuId, const std::string& name);

    static std::map<int, CPUTimes> m_CpuUsageDeltas;
    static std::optional<std::string> m_CpuTempPath;

    SystemInfoApi() = default;
};


#endif //WSYSMON_SYSTEMINFOAPI_H
