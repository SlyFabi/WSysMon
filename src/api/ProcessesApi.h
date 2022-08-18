#ifndef WSYSMON_PROCESSESAPI_H
#define WSYSMON_PROCESSESAPI_H

#include <string>
#include <vector>
#include <regex>

#include "../utils/IOUtils.h"
#include "SystemInfoApi.h"

struct ProcessCPUTimes {
    long utime;
    long stime;
    long cutime;
    long cstime;
    long starttime;
};

struct ProcessUserIds {
    int uid;
    int euid;
    int suid;
};

struct ProcessInfo {
    int pid;
    int parentPid;
    std::string name;
    std::string path;

    ProcessUserIds uids;
    ProcessCPUTimes cpuTimes;
    int numThreads;

    long memoryUsageBytes;

    long ioBytesRead;
    long ioBytesWritten;
};

class ProcessesApi {
public:
    static std::vector<ProcessInfo> GetAllProcesses();

    static int GetNumProcesses();
    static int GetNumThreads();

private:
    ProcessesApi() = default;
};


#endif //WSYSMON_PROCESSESAPI_H
