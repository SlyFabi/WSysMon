#include "../ProcessesApi.h"
#import <sys/sysctl.h>
#import <sys/proc_info.h>
#import <libproc.h>

// Thanks to chromium (https://chromium.googlesource.com/crashpad/crashpad/+/refs/heads/master/util/posix/process_info_mac.cc)
inline bool parse_args(ProcessInfo& proc) {
    size_t args_size_estimate;
    size_t args_size;
    std::string args;
    int tries = 3;
    const pid_t pid = proc.pid;
    do {
        int mib[] = {CTL_KERN, KERN_PROCARGS2, pid};
        int rv = sysctl(mib, std::size(mib), nullptr, &args_size_estimate, nullptr, 0);
        if (rv != 0) {
            return false;
        }

        args_size = args_size_estimate + 32;
        args.resize(args_size);
        rv = sysctl(mib, std::size(mib), &args[0], &args_size, nullptr, 0);
        if (rv != 0) {
            return false;
        }
    } while (args_size == args_size_estimate + 1 && tries--);
    if (args_size == args_size_estimate + 1) {
        return false;
    }

    // KERN_PROCARGS2 needs to at least contain argc.
    if (args_size < sizeof(int)) {
        return false;
    }
    args.resize(args_size);

    // Get argc.
    int argc;
    memcpy(&argc, &args[0], sizeof(argc));

    // Find the end of the executable path.
    size_t start_pos = sizeof(argc);
    size_t nul_pos = args.find('\0', start_pos);
    if (nul_pos == std::string::npos) {
        return false;
    }

    // Find the beginning of the string area.
    start_pos = args.find_first_not_of('\0', nul_pos);
    if (start_pos == std::string::npos) {
        return false;
    }

    std::vector<std::string> local_argv;
    while (argc-- && nul_pos != std::string::npos) {
        nul_pos = args.find('\0', start_pos);
        local_argv.push_back(args.substr(start_pos, nul_pos - start_pos));
        start_pos = nul_pos + 1;
    }

    proc.path = Utils::stringReplace(local_argv[0], "\\", "/");
    proc.name = proc.path.substr(proc.path.find_last_of('/') + 1);
    return true;
}

std::vector<ProcessInfo> ProcessesApi::GetAllProcesses() {
    static int maxArgumentSize = 0;
    if (maxArgumentSize == 0) {
        size_t size = sizeof(maxArgumentSize);
        if (sysctl((int[]){ CTL_KERN, KERN_ARGMAX }, 2, &maxArgumentSize, &size, nullptr, 0) == -1) {
            maxArgumentSize = 4096; // Default
        }
    }

    std::vector<ProcessInfo> procList;
    int mib[3] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL};
    struct kinfo_proc *info;
    size_t length;
    size_t count;

    if (sysctl(mib, 3, nullptr, &length, nullptr, 0) < 0)
        return {};
    if (!(info = static_cast<kinfo_proc *>(malloc(length))))
        return {};
    if (sysctl(mib, 3, info, &length, nullptr, 0) < 0) {
        free(info);
        return {};
    }

    count = length / sizeof(struct kinfo_proc);
    for (int i = 0; i < count; i++) {
        pid_t pid = info[i].kp_proc.p_pid;
        if (pid == 0) {
            continue;
        }

        auto procInfo = ProcessInfo();
        procInfo.pid = pid;
        procInfo.parentPid = info[i].kp_eproc.e_ppid;
        procInfo.uids.uid = (int)info[i].kp_eproc.e_pcred.p_ruid;
        procInfo.uids.euid = (int)info[i].kp_eproc.e_ucred.cr_uid;
        procInfo.uids.suid = (int)info[i].kp_eproc.e_pcred.p_svuid;
        parse_args(procInfo);

        struct proc_taskallinfo allInfo{};
        if(proc_pidinfo(pid, PROC_PIDTASKALLINFO, 0, (void*) &allInfo, sizeof(struct proc_taskallinfo)) == PROC_PIDTASKALLINFO_SIZE) {
            procInfo.memoryUsageBytes = (long)allInfo.ptinfo.pti_resident_size;
            procInfo.numThreads = allInfo.ptinfo.pti_threadnum;
        }

        procList.emplace_back(procInfo);
    }

    free(info);
    return procList;
}

int ProcessesApi::GetNumProcesses() {
    return (int)GetAllProcesses().size();
}

int ProcessesApi::GetNumThreads() {
    auto numThreads = 0;
    for(const auto& proc : GetAllProcesses())
        numThreads += proc.numThreads;
    return numThreads;
}