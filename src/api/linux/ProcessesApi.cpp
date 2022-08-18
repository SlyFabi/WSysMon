#include "../ProcessesApi.h"

#include <filesystem>
#include <unistd.h>

inline bool parse_stat(const std::string& path, ProcessInfo& proc) {
    auto file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        return false;
    }

    char status;
    char *name = (char*) calloc(sizeof(char), 200);
    int tmpI;
    long tmpL;
    int scanResult = fscanf(file,
                            "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu "
                            "%ld %ld %ld %ld %ld %ld %lu",
                            &proc.pid, name, &status, &proc.parentPid, &tmpI, &tmpI,
                            &tmpI, &tmpI, &tmpI, &tmpL, &tmpL,
                            &tmpL, &tmpL, &proc.cpuTimes.utime, &proc.cpuTimes.stime, &proc.cpuTimes.cutime,
                            &proc.cpuTimes.cstime, &tmpL, &tmpL, &tmpL,
                            &tmpL, &proc.cpuTimes.starttime);
    if(scanResult != 22) {
        free(name);
        fclose(file);
        return false;
    }

    proc.name = std::string(name);
    proc.name = proc.name.substr(1, proc.name.size() - 2);
    free(name);
    fclose(file);
    return true;
}

inline bool parse_status(const std::string& path, ProcessInfo& proc) {
    auto statusLines = IOUtils::ReadAllLines(path);
    bool parsed = false;

    char *entryName = (char*) calloc(sizeof(char), 200);
    int uid;
    int ruid;
    int euid;
    int tuid;
    int numThreads;
    for(const auto& line : statusLines) {
        int scanResult = sscanf(line.c_str(),"%s\t%d", entryName, &numThreads);
        if(scanResult == 2 && std::string(entryName) == "Threads:") {
            proc.numThreads = numThreads;
        }

        scanResult = sscanf(line.c_str(),"%s\t%d\t%d\t%d\t%d", entryName, &uid, &ruid, &euid, &tuid);
        if(scanResult == 5 && std::string(entryName) == "Uid:") {
            proc.uids.uid = uid;
            proc.uids.euid = euid;
            proc.uids.suid = tuid;
            parsed = true;
        }
    }

    free(entryName);
    return parsed;
}

inline bool parse_statm(const std::string& path, ProcessInfo& proc) {
    auto file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        return false;
    }

    long resident;
    long share;
    long tmpL;
    int scanResult = fscanf(file,"%lu %lu %lu", &tmpL, &resident, &share);
    if(scanResult != 3) {
        fclose(file);
        return false;
    }

    long page_size_bytes = sysconf(_SC_PAGE_SIZE);
    proc.memoryUsageBytes = resident * page_size_bytes - share * page_size_bytes;
    fclose(file);
    return true;
}

inline void parse_cmdline(const std::string& path, ProcessInfo& proc) {
    auto procPath = IOUtils::ReadAllTextNullByte(path);
    auto cmdSplit = Utils::stringSplit(procPath, " ");

    proc.path = procPath;
    if(!cmdSplit.empty()) {
        for(const auto& split : cmdSplit) {
            if(split.rfind("/", 0) == 0)
                proc.path = split;
        }
    }

    if(proc.path.empty())
        return;
    proc.path = Utils::stringReplace(proc.path, "\\", "/");
    proc.name = proc.path.substr(proc.path.find_last_of('/') + 1);
}

inline void parse_io(const std::string& path, ProcessInfo& proc) {
    auto ioLines = IOUtils::ReadAllLines(path);

    char *entryName = (char*) calloc(sizeof(char), 200);
    long entryVal;
    for(const auto& line : ioLines) {
        int scanResult = sscanf(line.c_str(),"%s %lu", entryName, &entryVal);
        if(scanResult != 2)
            continue;

        if(std::string(entryName) == "read_bytes:") {
            proc.ioBytesRead = entryVal;
        } else if(std::string(entryName) == "write_bytes:") {
            proc.ioBytesWritten = entryVal;
        }
    }

    free(entryName);
}

std::vector<ProcessInfo> ProcessesApi::GetAllProcesses() {
    std::vector<ProcessInfo> result;

    auto lambda = [](const std::string& s) { return std::all_of(s.begin(), s.end(), ::isdigit); };
    for (const auto& dirEntry : std::filesystem::directory_iterator("/proc")) {
        if (lambda(dirEntry.path().stem().string())) {
            auto procPath = dirEntry.path().string();
            ProcessInfo proc{};
            ProcessCPUTimes cpuTimes{};
            ProcessUserIds uids{};

            proc.cpuTimes = cpuTimes;
            proc.uids = uids;

            // Stat
            if(!parse_stat(procPath + "/stat", proc))
                continue;

            // Status
            if(!parse_status(procPath + "/status", proc))
                continue;

            // Statm
            if(!parse_statm(procPath + "/statm", proc))
                continue;

            // Cmdline
            parse_cmdline(procPath + "/cmdline", proc);

            // IO
            parse_io(procPath + "/io", proc);

            result.push_back(proc);
        }
    }

    return result;
}

int ProcessesApi::GetNumProcesses() {
    int count = 0;
    auto lambda = [](const std::string& s) { return std::all_of(s.begin(), s.end(), ::isdigit); };
    for (const auto& dirEntry : std::filesystem::directory_iterator("/proc")) {
        if (lambda(dirEntry.path().stem().string())) {
            count++;
        }
    }
    return count;
}

int ProcessesApi::GetNumThreads() {
    auto numThreads = 0;
    for(const auto& proc : GetAllProcesses())
        numThreads += proc.numThreads;
    return numThreads;
}
