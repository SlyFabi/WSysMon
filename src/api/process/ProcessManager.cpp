#include "ProcessManager.h"
#include "../../utils/X11Utils.h"
#include "../../storage/AppSettings.h"

#include <unistd.h>
#include <csignal>

std::vector<ProcessNode *> ProcessManager::m_AllProcessesCache;
std::map<int, std::vector<ProcessNode *>> ProcessManager::m_CategoryCache;
std::map<int, ProcessNode *> ProcessManager::m_CategoryTreeCache;

std::map<int, ProcessCPUTimes> ProcessManager::m_PidCpuTimes;
std::map<int, ProcessIOUsage> ProcessManager::m_PidIOUsages;

std::vector<std::string> g_AppBlacklist = {"gnome-shell", "plasmashell"}; // NOLINT(cert-err58-cpp)

inline bool is_in_app_blacklist(ProcessNode *node) {
    return Utils::vectorContains(g_AppBlacklist, node->GetName());
}

void ProcessManager::ClearCache() {
    for(auto node : m_AllProcessesCache)
        delete node;
    for(const auto& pair : m_CategoryCache)
        for(auto node : pair.second)
            delete node;
    for(auto pair : m_CategoryTreeCache)
        delete pair.second;

    m_AllProcessesCache.clear();
    m_CategoryCache.clear();
    m_CategoryTreeCache.clear();
}

ProcessNode *ProcessManager::GetProcessTreeByCategory(int categoryId) {
    if(Utils::mapContains(m_CategoryTreeCache, categoryId))
        return m_CategoryTreeCache[categoryId];

    ProcessNode *root;
    auto settings = AppSettings::Get();
    if(settings.displayProcList) {
        root = ProcessNode::BuildPackedFlatTree(GetProcessesByCategory(categoryId));
    } else {
        root = ProcessNode::BuildTree(GetProcessesByCategory(categoryId));
    }

    root->CalculateCPUUsageSums();
    root->CalculateRAMUsageSums();
    root->CalculateDiskUsageSums();
    root->CalculateGPUUsageSums();

    m_CategoryTreeCache[categoryId] = root;
    return root;
}

ProcessNode *ProcessManager::GetProcessByPid(int pid) {
    for(auto node : m_AllProcessesCache)
        if(node->GetPid() == pid)
            return node;
    return nullptr;
}

void ProcessManager::KillByPid(int pid, bool force) {
    int signal = force ? SIGKILL : SIGTERM;
    if(kill(pid, signal) != 0) {
        spdlog::error("Error killing {}", pid);
    }
}

std::vector<ProcessNode *> ProcessManager::GetProcessesByCategory(int categoryId) {
    if(Utils::mapContains(m_CategoryCache, categoryId))
        return m_CategoryCache[categoryId];

    std::vector<ProcessNode *> result;
    std::function<bool(ProcessNode *)> filterFunc;
    switch(categoryId) {
        case PROCESSES_VIEW_CATEGORY_APPS: {
            auto windowPids = X11Utils::GetAllPidsWithWindows();
            filterFunc = [&](ProcessNode *proc) -> bool { return proc->GetUserIds().uid == getuid() && !is_in_app_blacklist(proc) && Utils::vectorContains(windowPids, proc->GetPid()); };
            result = ProcessManager::GetProcessesByFilter(filterFunc, !AppSettings::Get().displayProcList);
            break;
        }
        case PROCESSES_VIEW_CATEGORY_WINE: {
            filterFunc = [](ProcessNode *proc) -> bool { return Utils::stringEndsWith(proc->GetName(), ".exe"); };
            result = ProcessManager::GetProcessesByFilter(filterFunc);

            ProcessNode::Reduce(result, GetProcessesByCategory(PROCESSES_VIEW_CATEGORY_APPS));
            break;
        }
        case PROCESSES_VIEW_CATEGORY_BACKGROUND: {
            filterFunc = [](ProcessNode *proc) -> bool { return proc->GetUserIds().uid == getuid(); };
            result = ProcessManager::GetProcessesByFilter(filterFunc);

            ProcessNode::Reduce(result, GetProcessesByCategory(PROCESSES_VIEW_CATEGORY_APPS));
            ProcessNode::Reduce(result, GetProcessesByCategory(PROCESSES_VIEW_CATEGORY_WINE));
            break;
        }
        case PROCESSES_VIEW_CATEGORY_SYSTEM: {
            filterFunc = [](ProcessNode *proc) -> bool { return proc->GetUserIds().uid != getuid(); };
            result = ProcessManager::GetProcessesByFilter(filterFunc);

            ProcessNode::Reduce(result, GetProcessesByCategory(PROCESSES_VIEW_CATEGORY_APPS));
            ProcessNode::Reduce(result, GetProcessesByCategory(PROCESSES_VIEW_CATEGORY_WINE));
            ProcessNode::Reduce(result, GetProcessesByCategory(PROCESSES_VIEW_CATEGORY_BACKGROUND));
            break;
        }
        default: {
            spdlog::warn("ProcessManager unhandled category !");
            break;
        }
    }

    m_CategoryCache[categoryId] = result;
    return result;
}

std::vector<ProcessNode *> ProcessManager::GetProcessesByFilter(const std::function<bool(ProcessNode *)>& filterFunc, bool withChildren) {
    std::vector<ProcessNode *> result;
    std::vector<int> pids;

    auto procList = GetAllProcesses();
    for(auto proc : procList) {
        if(filterFunc(proc)) {
            for(auto child : proc->FlatTree()) {
                if(Utils::vectorContains(pids, child->GetPid()))
                    continue;
                if(!withChildren && proc->GetName() != child->GetName())
                    continue;

                result.push_back(child->Copy());
                pids.push_back(child->GetPid());
            }
        }
    }

    return result;
}

std::vector<ProcessNode *> ProcessManager::GetAllProcesses() {
    if(!m_AllProcessesCache.empty())
        return m_AllProcessesCache;

    std::vector<ProcessNode *> result;

    auto procList = ProcessesApi::GetAllProcesses();
    auto cpuTimes = SystemInfoApi::GetCPUTimes(-1);

    auto nvidiaInfo = NvGpuApi::GetInfo();
    for(const auto& proc : procList) {
        auto cpuUsage = GetCPUUsageForProc(proc, cpuTimes);
        auto ioUsage = GetIOUsageForProc(proc);

        ProcessDetails details{};
        details.path = proc.path;

        GPUProcessInfo gpuInfo{};
        if(nvidiaInfo.has_value())
            gpuInfo = GPUApi::GetGPUProcessInfo(nvidiaInfo.value(), 0, proc.pid);

        auto node = new ProcessNode(proc.pid, proc.parentPid, proc.name, "", proc.uids, details,
                                    cpuUsage, proc.memoryUsageBytes, ioUsage.diskUsageBytes, 0,
                                    proc.ioBytesRead, proc.ioBytesWritten, gpuInfo);
        result.push_back(node);

        UpdateCPUTimeForProc(proc, cpuTimes);
        UpdateIOUsageForProc(proc);
    }

    m_AllProcessesCache = result;
    delete ProcessNode::BuildTree(m_AllProcessesCache);
    return result;
}

double ProcessManager::GetCPUUsageForProc(const ProcessInfo& proc, CPUTimes cpuTimes) {
    if(!Utils::mapContains(m_PidCpuTimes, proc.pid))
        return 0.F;

    auto procTimes = proc.cpuTimes;
    auto lastTimes = m_PidCpuTimes[proc.pid];

    auto utimeDiff = procTimes.utime + procTimes.cutime - lastTimes.utime - lastTimes.cutime;
    auto stimeDiff = procTimes.stime + procTimes.cstime - lastTimes.stime - lastTimes.cstime;

    auto total = cpuTimes.user + cpuTimes.nice + cpuTimes.system + cpuTimes.idle + cpuTimes.iowait + cpuTimes.irq + cpuTimes.softirq + cpuTimes.steal;
    auto lastTotal = lastTimes.starttime;
    auto totalDiff = total - lastTotal;

    return 100. * (double)utimeDiff / (double)totalDiff + 100. * (double)stimeDiff / (double)totalDiff;
}

void ProcessManager::UpdateCPUTimeForProc(const ProcessInfo& proc, CPUTimes cpuTimes) {
    auto total = cpuTimes.user + cpuTimes.nice + cpuTimes.system + cpuTimes.idle + cpuTimes.iowait + cpuTimes.irq + cpuTimes.softirq + cpuTimes.steal;
    m_PidCpuTimes[proc.pid].utime =  proc.cpuTimes.utime;
    m_PidCpuTimes[proc.pid].stime =  proc.cpuTimes.stime;
    m_PidCpuTimes[proc.pid].cutime =  proc.cpuTimes.cutime;
    m_PidCpuTimes[proc.pid].cstime =  proc.cpuTimes.cstime;
    m_PidCpuTimes[proc.pid].starttime = total;
}

ProcessIOUsage ProcessManager::GetIOUsageForProc(const ProcessInfo& proc) {
    if(!Utils::mapContains(m_PidIOUsages, proc.pid))
        return {};

    auto lastUsage = m_PidIOUsages[proc.pid];
    auto diskUsageBytes = proc.ioBytesRead + proc.ioBytesWritten;

    auto timeDiffS = (double)(Utils::GetCurrentTimeMS() - lastUsage.timeMs) / 1000.;
    auto bytesDiff = diskUsageBytes - lastUsage.diskUsageBytes;

    auto mul = 1.;
    if(timeDiffS < 1)
        mul += timeDiffS;
    bytesDiff = (long)((double)bytesDiff * mul);

    auto usage = ProcessIOUsage();
    usage.diskUsageBytes = bytesDiff;
    return usage;
}

void ProcessManager::UpdateIOUsageForProc(const ProcessInfo& proc) {
    auto usage = ProcessIOUsage();
    usage.diskUsageBytes = proc.ioBytesRead + proc.ioBytesWritten;
    usage.timeMs = Utils::GetCurrentTimeMS();
    m_PidIOUsages[proc.pid] = usage;
}
