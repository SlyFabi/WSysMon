#ifndef WSYSMON_PROCESSMANAGER_H
#define WSYSMON_PROCESSMANAGER_H

#include <functional>
#include <map>
#include <vector>

#include "ProcessNode.h"
#include "../ProcessesApi.h"

#define PROCESSES_VIEW_CATEGORY_UNKNOWN 0
#define PROCESSES_VIEW_CATEGORY_APPS 1
#define PROCESSES_VIEW_CATEGORY_WINE 2
#define PROCESSES_VIEW_CATEGORY_BACKGROUND 3
#define PROCESSES_VIEW_CATEGORY_SYSTEM 4

struct ProcessIOUsage {
    long diskUsageBytes;
    long networkUsageBytes;
    long timeMs;
};

class ProcessManager {
public:
    static void ClearCache();
    static ProcessNode *GetProcessTreeByCategory(int categoryId);
    static ProcessNode *GetProcessByPid(int pid);
    static ProcessNode *GetTreeProcessByPid(int pid);

    static void KillByPid(int pid, bool force = false);
private:
    static std::vector<ProcessNode *> GetProcessesByCategory(int categoryId);
    static std::vector<ProcessNode *> GetProcessesByFilter(const std::function<bool(ProcessNode *)>& filterFunc, bool withChildren = true);
    static std::vector<ProcessNode *> GetAppProcessesByFilter(const std::function<bool(ProcessNode *)>& filterFunc, bool withChildren = true);
    static std::vector<ProcessNode *> GetAllProcesses();

    static double GetCPUUsageForProc(const ProcessInfo& proc, CPUTimes cpuTimes);
    static void UpdateCPUTimeForProc(const ProcessInfo& proc, CPUTimes cpuTimes);

    static ProcessIOUsage GetIOUsageForProc(const ProcessInfo& proc);
    static void UpdateIOUsageForProc(const ProcessInfo& proc);

    static std::vector<ProcessNode *> m_AllProcessesCache;
    static std::map<int, std::vector<ProcessNode *>> m_CategoryCache;
    static std::map<int, ProcessNode *> m_CategoryTreeCache;
    static std::vector<ProcessNode *> m_TreeProcessesCache;

    static std::map<int, ProcessCPUTimes> m_PidCpuTimes;
    static std::map<int, ProcessIOUsage> m_PidIOUsages;

    ProcessManager() = default;
};


#endif //WSYSMON_PROCESSMANAGER_H
