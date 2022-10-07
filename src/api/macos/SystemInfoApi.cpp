#include "../SystemInfoApi.h"
#import <sys/sysctl.h>
#import <mach/mach.h>

std::map<int, CPUTimes> SystemInfoApi::m_CpuUsageDeltas;

long SystemInfoApi::GetTotalRam() {
    long val;
    size_t len = sizeof(val);
    sysctlbyname("hw.memsize", &val, &len, nullptr, 0);
    return val;
}

long SystemInfoApi::GetFreeRam() {
    return GetTotalRam() - GetUsedRam();
}

long SystemInfoApi::GetUsedRam() {
    vm_statistics64_data_t  vm_stat;
    mach_port_t host;
    vm_size_t pageSize = 4096;
    host = mach_host_self();
    if(host_page_size(mach_host_self(), &pageSize) != KERN_SUCCESS) {
        pageSize = 4096;
    }

    unsigned int count = HOST_VM_INFO64_COUNT;
    if (host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm_stat,
                          &count) == KERN_SUCCESS) {
        return (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * (long)pageSize;
    }
    return 0;
}

double SystemInfoApi::GetRamUsagePercent() {
    auto totalRam = (double)SystemInfoApi::GetTotalRam();
    auto usedRam = (double)SystemInfoApi::GetUsedRam();
    return usedRam / totalRam;
}

CPUTimes SystemInfoApi::GetCPUTimes(int cpuId) {
    processor_info_array_t cpuInfo;
    mach_msg_type_number_t numCpuInfo;
    natural_t numCPUsU = 0U;
    kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &cpuInfo, &numCpuInfo);
    if(err != KERN_SUCCESS) {
        return {};
    }

    auto cpuTimes = CPUTimes();
    if(cpuId >= 0) {
        cpuTimes.user = cpuInfo[(CPU_STATE_MAX * cpuId) + CPU_STATE_USER];
        cpuTimes.nice = cpuInfo[(CPU_STATE_MAX * cpuId) + CPU_STATE_NICE];
        cpuTimes.system = cpuInfo[(CPU_STATE_MAX * cpuId) + CPU_STATE_SYSTEM];
        cpuTimes.idle = cpuInfo[(CPU_STATE_MAX * cpuId) + CPU_STATE_IDLE];
    } else {
        for(int i = 0; i < numCPUsU; i++) {
            cpuTimes.user += cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER];
            cpuTimes.nice += cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE];
            cpuTimes.system += cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM];
            cpuTimes.idle += cpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE];
        }
    }

    return cpuTimes;
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
    return 0;
}

long SystemInfoApi::GetMaxCPUClock() {
    long val;
    size_t len = sizeof(val);
    sysctlbyname("hw.cpufrequency_max", &val, &len, nullptr, 0);
    return val;
}

double SystemInfoApi::GetCPUTemperature() {
    return 0;
}

std::string SystemInfoApi::GetCPUName() {
    char *val;
    size_t len;
    sysctlbyname("machdep.cpu.brand_string", nullptr, &len, nullptr, 0);
    val = static_cast<char *>(malloc(len));
    sysctlbyname("machdep.cpu.brand_string", val, &len, nullptr, 0);
    auto res = std::string(val, len);
    free(val);
    return res;
}

int SystemInfoApi::GetNumCPUs() {
    int val;
    size_t len = sizeof(val);
    sysctlbyname("hw.physicalcpu", &val, &len, nullptr, 0);
    return val;
}

int SystemInfoApi::GetNumCPUsLogical() {
    int val;
    size_t len = sizeof(val);
    sysctlbyname("hw.logicalcpu", &val, &len, nullptr, 0);
    return val;
}

int SystemInfoApi::GetSocketCount() {
    return 0;
}

long SystemInfoApi::GetUptimeMS() {
    return 0;
}

long SystemInfoApi::ReadNamedEntry(const std::string& path, const std::string& name) {
    return 0;
}

std::string SystemInfoApi::ReadCPUEntry(int cpuId, const std::string& name) {
    return {};
}
